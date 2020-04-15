/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#include "vtkSlicerFreeSurferParcellateRule.h"

#include "vtkMRMLMeshModifyNode.h"

// MRML includes
#include <vtkMRMLMarkupsCurveNode.h>
#include <vtkMRMLMarkupsPlaneNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLTransformNode.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkCollection.h>
#include <vtkConnectivityFilter.h>
#include <vtkCommand.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkGeneralTransform.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkPolyDataNormals.h>
#include <vtkSelectPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>

//----------------------------------------------------------------------------
vtkRuleNewMacro(vtkSlicerFreeSurferParcellateRule);

//----------------------------------------------------------------------------
vtkSlicerFreeSurferParcellateRule::vtkSlicerFreeSurferParcellateRule()
{
  /////////
  // Inputs
  vtkNew<vtkIntArray> inputPlaneEvents;
  inputPlaneEvents->InsertNextTuple1(vtkCommand::ModifiedEvent);
  inputPlaneEvents->InsertNextTuple1(vtkMRMLMarkupsNode::PointModifiedEvent);
  inputPlaneEvents->InsertNextTuple1(vtkMRMLTransformableNode::TransformModifiedEvent);

  NodeInfo inputPlane1(
    "Plane node 1",
    "Plane node used to define sulci for grey matter parcellation.",
    "vtkMRMLMarkupsPlaneNode",
    "PlaneCut.InputPlane1",
    true,
    inputPlaneEvents
    );
  this->InputNodeInfo.push_back(inputPlane1);

  NodeInfo inputPlane2(
    "Plane node 2",
    "Plane node used to define sulci for grey matter parcellation.",
    "vtkMRMLMarkupsPlaneNode",
    "PlaneCut.InputPlane2",
    true,
    inputPlaneEvents
  );
  this->InputNodeInfo.push_back(inputPlane2);

  NodeInfo inputCurve1(
    "Curve node 1",
    "Curve node used to trace sulci for grey matter parcellation.",
    "vtkMRMLMarkupsCurveNode",
    "PlaneCut.InputCurve1",
    true,
    inputPlaneEvents
  );
  this->InputNodeInfo.push_back(inputCurve1);

  NodeInfo inputCurve2(
    "Curve node 2",
    "Curve node used to trace sulci for grey matter parcellation.",
    "vtkMRMLMarkupsCurveNode",
    "PlaneCut.InputCurve2",
    true,
    inputPlaneEvents
  );
  this->InputNodeInfo.push_back(inputCurve2);

  vtkNew<vtkIntArray> inputModelEvents;
  inputModelEvents->InsertNextTuple1(vtkCommand::ModifiedEvent);
  inputModelEvents->InsertNextTuple1(vtkMRMLModelNode::MeshModifiedEvent);
  inputModelEvents->InsertNextTuple1(vtkMRMLTransformableNode::TransformModifiedEvent);

  NodeInfo origModel(
    "Orig model node",
    "Orig model node from FreeSurfer.",
    "vtkMRMLModelNode",
    "PlaneCut.InputOrigModel",
    true,
    inputModelEvents
    );
  this->InputNodeInfo.push_back(origModel);

  NodeInfo pialModel(
    "Pial model node",
    "Pial model node from FreeSurfer.",
    "vtkMRMLModelNode",
    "PlaneCut.InputPialModel",
    true,
    inputModelEvents
  );
  this->InputNodeInfo.push_back(pialModel);

  /////////
  // Outputs
  NodeInfo outputParcellationModel(
    "Parcellation",
    "Parcellated gray matter model.",
    "vtkMRMLModelNode",
    "PlaneCut.OutputParcellation",
    false
    );
  this->OutputNodeInfo.push_back(outputParcellationModel);

}

//----------------------------------------------------------------------------
vtkSlicerFreeSurferParcellateRule::~vtkSlicerFreeSurferParcellateRule()
{
}

//----------------------------------------------------------------------------
const char* vtkSlicerFreeSurferParcellateRule::GetName()
{
  return "FreeSurfer parcellate";
}

//----------------------------------------------------------------------------
bool vtkSlicerFreeSurferParcellateRule::RunInternal(vtkMRMLMeshModifyNode* meshModifyNode)
{
  if (!this->HasRequiredInputs(meshModifyNode))
    {
    vtkErrorMacro("Invalid number of inputs");
    return false;
    }

  vtkMRMLModelNode* outputModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthOutputNode(0, meshModifyNode)); // TODO: nth order is never defined
  if (!outputModelNode)
    {
    // Nothing to output
    return true;
    }

  int i = 0;
  vtkMRMLMarkupsPlaneNode* inputPlane1Node = vtkMRMLMarkupsPlaneNode::SafeDownCast(this->GetNthInputNode(i++, meshModifyNode)); // TODO: nth order is never defined
  vtkMRMLMarkupsPlaneNode* inputPlane2Node = vtkMRMLMarkupsPlaneNode::SafeDownCast(this->GetNthInputNode(i++, meshModifyNode));
  vtkMRMLMarkupsCurveNode* inputCurve1Node = vtkMRMLMarkupsCurveNode::SafeDownCast(this->GetNthInputNode(i++, meshModifyNode));
  vtkMRMLMarkupsCurveNode* inputCurve2Node = vtkMRMLMarkupsCurveNode::SafeDownCast(this->GetNthInputNode(i++, meshModifyNode));
  vtkMRMLModelNode*        origModelNode   = vtkMRMLModelNode::SafeDownCast(this->GetNthInputNode(i++, meshModifyNode));
  vtkMRMLModelNode*        pialModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthInputNode(i++, meshModifyNode));
  if (!inputPlane1Node || !inputPlane2Node ||
      !inputCurve1Node || !inputCurve2Node ||
      !origModelNode   || !pialModelNode)
    {
    vtkErrorMacro("Invalid input nodes!");
    return false;
    }

  //if (!inputModelNode->GetMesh() || inputModelNode->GetMesh()->GetNumberOfPoints() == 0)
  //  {
  //  inputModelNode->GetMesh()->Initialize();
  //  return true;
  //  }

  //if (inputModelNode->GetParentTransformNode())
  //  {
  //  inputModelNode->GetParentTransformNode()->GetTransformToWorld(this->InputModelNodeToWorldTransform);
  //  }
  //if (outputPositiveModelNode && outputPositiveModelNode->GetParentTransformNode())
  //  {
  //  outputPositiveModelNode->GetParentTransformNode()->GetTransformFromWorld(this->OutputPositiveWorldToModelTransform);
  //  }
  //if (outputNegativeModelNode && outputNegativeModelNode->GetParentTransformNode())
  //  {
  //  outputNegativeModelNode->GetParentTransformNode()->GetTransformFromWorld(this->OutputNegativeWorldToModelTransform);
  //  }

  //this->InputModelToWorldTransformFilter->SetInputConnection(inputModelNode->GetMeshConnection()); 

  //double origin_World[3] = { 0.0 };
  //inputPlaneNode->GetOriginWorld(origin_World);
  //this->Plane->SetOrigin(origin_World);
  
  double normal1_World[3] = { 0.0 };
  inputPlane1Node->GetNormalWorld(normal1_World);
  double origin1_World[3] = { 0.0 };
  inputPlane1Node->GetOriginWorld(origin1_World);
  
  double normal2_World[3] = { 0.0 };
  inputPlane2Node->GetNormalWorld(normal2_World);
  double origin2_World[3] = { 0.0 };
  inputPlane2Node->GetOriginWorld(origin2_World);

  double plane1ToPlane2Vector_World[3] = { 0.0 };
  vtkMath::Subtract(origin2_World, origin1_World, plane1ToPlane2Vector_World);
  if (vtkMath::Dot(plane1ToPlane2Vector_World, normal1_World) < 0)
    {
    vtkMath::MultiplyScalar(normal1_World, -1);
    }
  if (vtkMath::Dot(plane1ToPlane2Vector_World, normal2_World) > 0)
    {
    vtkMath::MultiplyScalar(normal2_World, -1);
    }
  
  vtkNew<vtkGeneralTransform> pialTransformToWorld;
  if (pialModelNode->GetParentTransformNode())
    {
    pialModelNode->GetParentTransformNode()->GetTransformToWorld(pialTransformToWorld);
    }
  vtkNew<vtkTransformPolyDataFilter> pialModelToWorldTransform;
  pialModelToWorldTransform->SetInputData(pialModelNode->GetMesh());
  pialModelToWorldTransform->SetTransform(pialTransformToWorld);
  pialModelToWorldTransform->Update();

  vtkNew<vtkGeneralTransform> origTransformToWorld;
  if (origModelNode->GetParentTransformNode())
    {
    origModelNode->GetParentTransformNode()->GetTransformToWorld(origTransformToWorld);
    }
  vtkNew<vtkTransformPolyDataFilter> origModelToWorldTransform;
  origModelToWorldTransform->SetInputData(origModelNode->GetMesh());
  origModelToWorldTransform->SetTransform(origTransformToWorld);
  origModelToWorldTransform->Update();

  vtkNew<vtkPoints> model1Curve1Points;
  model1Curve1Points->DeepCopy(inputCurve1Node->GetCurvePointsWorld());
  vtkNew<vtkPoints> model1Curve2Points;
  model1Curve2Points->DeepCopy(inputCurve2Node->GetCurvePointsWorld());
  vtkNew<vtkPoints> model2Curve1Points;
  this->ProjectCurveToSurface(model1Curve1Points, origModelToWorldTransform->GetOutput(), pialModelToWorldTransform->GetOutput(), model2Curve1Points);
  vtkNew<vtkPoints> model2Curve2Points;
  this->ProjectCurveToSurface(model1Curve2Points, origModelToWorldTransform->GetOutput(), pialModelToWorldTransform->GetOutput(), model2Curve2Points);
  
  vtkNew<vtkPlane> plane1_World;
  plane1_World->SetNormal(normal1_World);
  plane1_World->SetOrigin(origin1_World);
  
  vtkNew<vtkPlane> plane2_World;
  plane2_World->SetNormal(normal2_World);
  plane2_World->SetOrigin(origin2_World);
  
  this->ClipCurveWithPlane(model1Curve1Points, plane1_World, model1Curve1Points);
  this->ClipCurveWithPlane(model1Curve1Points, plane2_World, model1Curve1Points);
  
  this->ClipCurveWithPlane(model1Curve2Points, plane1_World, model1Curve2Points);
  this->ClipCurveWithPlane(model1Curve2Points, plane2_World, model1Curve2Points);
  
  this->ClipCurveWithPlane(model2Curve1Points, plane1_World, model2Curve1Points);
  this->ClipCurveWithPlane(model2Curve1Points, plane2_World, model2Curve1Points);
  
  this->ClipCurveWithPlane(model2Curve2Points, plane1_World, model2Curve2Points);
  this->ClipCurveWithPlane(model2Curve2Points, plane2_World, model2Curve2Points);
    
  if (model1Curve1Points->GetNumberOfPoints() == 0)
    {
    return false;
    }
  if (model1Curve2Points->GetNumberOfPoints() == 0)
    {
    return false;
    }
  if (model2Curve1Points->GetNumberOfPoints() == 0)
    {
    return false;
    }
  if (model2Curve2Points->GetNumberOfPoints() == 0)
    {
    return false;
    }

  vtkNew<vtkClipPolyData> planeModel1Plane1Clip;
  planeModel1Plane1Clip->SetInputData(origModelToWorldTransform->GetOutput());
  planeModel1Plane1Clip->SetClipFunction(plane1_World);
  vtkNew<vtkClipPolyData> planeModel1Plane2Clip;
  planeModel1Plane2Clip->SetInputConnection(planeModel1Plane1Clip->GetOutputPort());
  planeModel1Plane2Clip->SetClipFunction(plane2_World);
  planeModel1Plane2Clip->Update();

  vtkNew<vtkPoints> model1ClosedCurve;
  this->CombineCurves(model1Curve1Points, model1Curve2Points, planeModel1Plane2Clip->GetOutput(), model1ClosedCurve);
  this->SetCurveClockwise(model1ClosedCurve);

  vtkNew<vtkClipPolyData> planeModel2Plane1Clip;
  planeModel2Plane1Clip->SetInputData(pialModelToWorldTransform->GetOutput());
  planeModel2Plane1Clip->SetClipFunction(plane1_World);
  vtkNew<vtkClipPolyData> planeModel2Plane2Clip;
  planeModel2Plane2Clip->SetInputConnection(planeModel2Plane1Clip->GetOutputPort());
  planeModel2Plane2Clip->SetClipFunction(plane2_World);
  planeModel2Plane2Clip->Update();

  vtkNew<vtkPoints> model2ClosedCurve;
  this->CombineCurves(model2Curve1Points, model2Curve2Points, planeModel2Plane2Clip->GetOutput(), model2ClosedCurve);
  this->SetCurveClockwise(model2ClosedCurve);


  vtkNew<vtkPolyData> model1Patch;
  this->GetSurfacePatch(model1ClosedCurve, planeModel1Plane2Clip->GetOutput(), model1Patch);

  vtkNew<vtkPolyDataNormals> flipNormalFilter;
  flipNormalFilter->SetInputData(model1Patch);
  flipNormalFilter->FlipNormalsOn();
  flipNormalFilter->Update();
  model1Patch->ShallowCopy(flipNormalFilter->GetOutput());

  vtkNew<vtkPolyData> model2Patch;
  this->GetSurfacePatch(model2ClosedCurve, planeModel2Plane2Clip->GetOutput(), model2Patch);
  
  vtkNew<vtkAppendPolyData> appendPolyDataFilter;
  appendPolyDataFilter->AddInputData(model1Patch);
  appendPolyDataFilter->AddInputData(model2Patch);
  appendPolyDataFilter->Update();

  vtkNew<vtkPolyData> outputPolyData;
  vtkNew<vtkIdList> line;
  for (int i = 0; i < model1ClosedCurve->GetNumberOfPoints(); ++i)
  {
    line->InsertNextId(i);
  }
  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(line);
  //outputPolyData->SetPoints(model1ClosedCurve);
  //outputPolyData->SetLines(lines);
  this->StitchEdges(appendPolyDataFilter->GetOutput(), model1ClosedCurve, model2ClosedCurve, outputPolyData);

  vtkNew<vtkGeneralTransform> outputTransformFromWorld;
  if (outputModelNode->GetParentTransformNode())
    {
    outputModelNode->GetParentTransformNode()->GetTransformFromWorld(outputTransformFromWorld);
    }
  vtkNew<vtkTransformPolyDataFilter> origModelFromWorldTransform;
  origModelFromWorldTransform->SetInputData(outputPolyData);
  origModelFromWorldTransform->SetTransform(outputTransformFromWorld);
  origModelFromWorldTransform->Update();

  MRMLNodeModifyBlocker blocker(outputModelNode);
  outputModelNode->SetAndObserveMesh(origModelFromWorldTransform->GetOutput());
  outputModelNode->InvokeCustomModifiedEvent(vtkMRMLModelNode::MeshModifiedEvent);
  
  return true;
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferParcellateRule::ProjectCurveToSurface(vtkPoints* inputCurve, vtkPolyData* originalSurface, vtkPolyData* projectedSurface, vtkPoints* outputProjectedPoints)
{
  // TODO: Argument checking
  vtkNew<vtkPointLocator> locator;
  locator->SetDataSet(originalSurface);
  locator->BuildLocator();
  for (int i = 0; i < inputCurve->GetNumberOfPoints(); ++i)
    {
    vtkIdType pointID = locator->FindClosestPoint(inputCurve->GetPoint(i));
    outputProjectedPoints->InsertNextPoint(projectedSurface->GetPoint(pointID));
    }
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferParcellateRule::ClipCurveWithPlane(vtkPoints* inputCurve, vtkPlane* clipPlane, vtkPoints* outputCurve)
{
  vtkNew<vtkPoints> outputPoints;
  for (int i = 0; i < inputCurve->GetNumberOfPoints() - 1; ++i)
  {
    double* point0 = inputCurve->GetPoint(i);
    double* point1 = inputCurve->GetPoint(i + 1);

    double point0Value = clipPlane->EvaluateFunction(point0);
    double point1Value = clipPlane->EvaluateFunction(point1);

    if (point0Value < 0 && point1Value < 0)
      {
      continue;
      }
    else if (point0Value >= 0 && point1Value >= 0)
      {
      outputPoints->InsertNextPoint(point0);
      }
    else if (point0Value == 0)
      {
      outputPoints->InsertNextPoint(point0);
      }
    else if (point0Value >= 0 && point1Value < 0)
      {
      double t = 0;
      double intersection[3] = { 0.0 };
      clipPlane->IntersectWithLine(point0, point1, t, intersection);
      outputPoints->InsertNextPoint(intersection);
      }
    else
      {
      outputPoints->InsertNextPoint(point0);
      }
    }
  outputCurve->ShallowCopy(outputPoints);
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferParcellateRule::CombineCurves(vtkPoints* inputCurve1, vtkPoints* inputCurve2, vtkPolyData* inputModel, vtkPoints* outputClosedCurve)
{
  vtkNew<vtkPointLocator> locator;
  locator->SetDataSet(inputModel);
  locator->BuildLocator();

  double curve1StartPoint[3] = { 0.0 };
  inputCurve1->GetPoint(0, curve1StartPoint);

  double curve1EndPoint[3] = { 0.0 };
  inputCurve1->GetPoint(inputCurve1->GetNumberOfPoints() - 1, curve1EndPoint);

  double curve2StartPoint[3] = { 0.0 };
  inputCurve2->GetPoint(0, curve2StartPoint);

  double curve2EndPoint[3] = { 0.0 };
  inputCurve2->GetPoint(inputCurve2->GetNumberOfPoints() - 1, curve2EndPoint);

  vtkNew<vtkPoints> reversedInputCurve2;
  if (vtkMath::Distance2BetweenPoints(curve1EndPoint, curve2EndPoint) < vtkMath::Distance2BetweenPoints(curve1EndPoint, curve1StartPoint))
    {
    for (int i = inputCurve2->GetNumberOfPoints() - 1; i > 0; --i)
      {
      reversedInputCurve2->InsertNextPoint(inputCurve2->GetPoint(i));
      }
    inputCurve2 = reversedInputCurve2;
    }

  vtkNew<vtkPoints> outputPoints;
  for (int i = 0; i < inputCurve1->GetNumberOfPoints(); ++i)
    {
    outputPoints->InsertNextPoint(inputCurve1->GetPoint(i));
    }

  vtkIdType point1IdEnd = locator->FindClosestPoint(inputCurve1->GetPoint(inputCurve1->GetNumberOfPoints() - 1));
  vtkIdType point2IdStart = locator->FindClosestPoint(inputCurve2->GetPoint(0));

  vtkNew<vtkDijkstraGraphGeodesicPath> dijkstra1;
  dijkstra1->SetInputData(inputModel);
  dijkstra1->SetStartVertex(point2IdStart);
  dijkstra1->SetEndVertex(point1IdEnd);
  dijkstra1->Update();
  vtkPolyData* dijkstraPath1 = dijkstra1->GetOutput();
  for (int i = 0; i < dijkstraPath1->GetNumberOfPoints(); ++i)
    {
    outputPoints->InsertNextPoint(dijkstraPath1->GetPoint(i));
    }

  for (int i = 0; i < inputCurve2->GetNumberOfPoints(); ++i)
    {
    outputPoints->InsertNextPoint(inputCurve2->GetPoint(i));
    }

  vtkIdType point1IdStart = locator->FindClosestPoint(inputCurve1->GetPoint(0));
  vtkIdType point2IdEnd = locator->FindClosestPoint(inputCurve2->GetPoint(inputCurve2->GetNumberOfPoints() - 1));

  vtkNew<vtkDijkstraGraphGeodesicPath> dijkstra2;
  dijkstra2->SetInputData(inputModel);
  dijkstra2->SetStartVertex(point1IdStart);
  dijkstra2->SetEndVertex(point2IdEnd);
  dijkstra2->Update();
  vtkPolyData* dijkstraPath2 = dijkstra2->GetOutput();
  for (int i = 0; i < dijkstraPath2->GetNumberOfPoints(); ++i)
    {
    outputPoints->InsertNextPoint(dijkstraPath2->GetPoint(i));
    }

  outputClosedCurve->ShallowCopy(outputPoints);
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferParcellateRule::GetSurfacePatch(vtkPoints* inputClosedCurve, vtkPolyData* inputModel, vtkPolyData* outputPatch)
{
  // Clip model with curve
  vtkNew<vtkSelectPolyData> selectFilter;
  selectFilter->SetInputData(inputModel);
  selectFilter->SetLoop(inputClosedCurve);
  selectFilter->GenerateSelectionScalarsOn();
  selectFilter->SetSelectionModeToLargestRegion();
  
  vtkNew<vtkClipPolyData> clip;
  clip->SetInputConnection(selectFilter->GetOutputPort());

  vtkNew <vtkConnectivityFilter> connectivity;
  connectivity->SetInputConnection(clip->GetOutputPort());

  vtkNew<vtkCleanPolyData> cleaner;
  cleaner->SetInputConnection(connectivity->GetOutputPort());
  cleaner->Update();
  outputPatch->ShallowCopy(cleaner->GetOutput());
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferParcellateRule::StitchEdges(vtkPolyData* inputPolyData, vtkPoints* inputCurve1, vtkPoints* inputCurve2, vtkPolyData* outputPolyData)
{
  vtkNew<vtkPointLocator> locator;
  locator->SetDataSet(inputPolyData);
  locator->BuildLocator();

  outputPolyData->DeepCopy(inputPolyData);

  vtkNew<vtkCellArray> polys;
  polys->DeepCopy(inputPolyData->GetPolys());

  vtkNew<vtkPolyData> curve2Poly;
  curve2Poly->SetPoints(inputCurve2);
  vtkNew<vtkPointLocator> locator2;
  locator2->SetDataSet(curve2Poly);
  locator2->BuildLocator();

  vtkIdType curve1StartId = 0;
  vtkIdType curve2StartId = locator2->FindClosestPoint(inputCurve1->GetPoint(0));
  vtkIdType curve1CurrentId = curve1StartId;
  vtkIdType curve2CurrentId = curve2StartId;
  bool curve1Initialized = false;
  bool curve2Initialized = false;
  for (int i = 0; i < inputCurve1->GetNumberOfPoints() + inputCurve2->GetNumberOfPoints() - 2; ++i)
    {
    vtkIdType curve1NextId = curve1CurrentId + 1;
    if (curve1NextId >= inputCurve1->GetNumberOfPoints() - 1)
      {
      curve1NextId = 0;
      }

    vtkIdType curve2NextId = curve2CurrentId + 1;
    if (curve2NextId >= inputCurve2->GetNumberOfPoints() - 1)
      {
      curve2NextId = 0;
      }

    double curve1CurrentPoint[3] = { 0.0 };
    inputCurve1->GetPoint(curve1CurrentId, curve1CurrentPoint);

    double curve2CurrentPoint[3] = { 0.0 };
    inputCurve2->GetPoint(curve2CurrentId, curve2CurrentPoint);

    double curve1NextPoint[3] = { 0.0 };
    inputCurve1->GetPoint(curve1NextId, curve1NextPoint);

    double curve2NextPoint[3] = { 0.0 };
    inputCurve2->GetPoint(curve2NextId, curve2NextPoint);

    vtkIdType curve1Point = locator->FindClosestPoint(curve1CurrentPoint);
    vtkIdType curve2Point = locator->FindClosestPoint(curve2CurrentPoint);
    vtkIdType connectingPointId = -1;
    if (curve1Initialized && curve1CurrentId == curve1StartId)
      {
      curve2CurrentId = curve2NextId;
      connectingPointId = locator->FindClosestPoint(curve2NextPoint);
      }
    else if (curve2Initialized && curve2CurrentId == curve2StartId)
      {
      curve1CurrentId = curve1NextId;
      connectingPointId = locator->FindClosestPoint(curve1NextPoint);
      }
    else
      {
      if (vtkMath::Distance2BetweenPoints(curve1NextPoint, curve2CurrentPoint) <
        vtkMath::Distance2BetweenPoints(curve2NextPoint, curve1CurrentPoint))
        {
        connectingPointId = locator->FindClosestPoint(curve1NextPoint);
        curve1CurrentId = curve1NextId;
        curve1Initialized = true;
        }
      else
        { 
        connectingPointId = locator->FindClosestPoint(curve2NextPoint);
        curve2CurrentId = curve2NextId;
        curve2Initialized = true;
        }
      }

    vtkNew<vtkIdList> triangle1;
    triangle1->InsertNextId(curve1Point);
    triangle1->InsertNextId(curve2Point);
    triangle1->InsertNextId(connectingPointId);
    polys->InsertNextCell(triangle1);
    }
  outputPolyData->SetPolys(polys);
}

//----------------------------------------------------------------------------5
void vtkSlicerFreeSurferParcellateRule::SetCurveClockwise(vtkPoints* curve)
{
  double sum = 0.0;
  for (int i = 0; i < curve->GetNumberOfPoints() - 1; ++i)
    {
    double p1[3] = { 0.0 };
    curve->GetPoint(i, p1);
    double p2[3] = { 0.0 };
    curve->GetPoint(i+1, p2);
    sum += (p2[0] - p1[0]) * (p2[1] + p1[1]);
    }

  if (sum <= 0.0)
    {
    return;
    }

  vtkNew<vtkPoints> reverseCurve;
  for (int i = 0; i < curve->GetNumberOfPoints(); ++i)
    {
    reverseCurve->InsertNextPoint(curve->GetPoint(i));
    }
  curve->ShallowCopy(reverseCurve);
}