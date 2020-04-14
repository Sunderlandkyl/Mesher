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

#include "vtkSlicerPlaneCutRule.h"

#include "vtkMRMLMeshModifyNode.h"

// MRML includes
#include <vtkMRMLMarkupsPlaneNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLTransformNode.h>

// VTK includes
#include <vtkClipPolyData.h>
#include <vtkCollection.h>
#include <vtkGeneralTransform.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>

//----------------------------------------------------------------------------
vtkRuleNewMacro(vtkSlicerPlaneCutRule);

//----------------------------------------------------------------------------
vtkSlicerPlaneCutRule::vtkSlicerPlaneCutRule()
{
  /////////
  // Inputs
  NodeInfo inputPlane(
    "Plane node",
    "Plane node to cut the model node.",
    "vtkMRMLMarkupsPlaneNode",
    "PlaneCut.InputPlane",
    true
    );
  this->InputNodeInfo.push_back(inputPlane);

  NodeInfo inputModel(
    "Model node",
    "Model node to be cut.",
    "vtkMRMLModelNode",
    "PlaneCut.InputModel",
    true
    );
  this->InputNodeInfo.push_back(inputModel);

  /////////
  // Outputs
  NodeInfo outputPositiveModel(
    "Positive model node",
    "Portion of the cut model that is on the same side of the plane as the normal.",
    "vtkMRMLModelNode",
    "PlaneCut.OutputPositiveModel",
    false
    );
  this->OutputNodeInfo.push_back(outputPositiveModel);

  NodeInfo outputNegativeModel(
    "Negative model node",
    "Portion of the cut model that is on the opposite side of the plane as the normal.",
    "vtkMRMLModelNode",
    "PlaneCut.OutputNegativeModel",
    false
    ); 
  this->OutputNodeInfo.push_back(outputNegativeModel);

  this->InputModelToWorldTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->InputModelNodeToWorldTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  this->InputModelToWorldTransformFilter->SetTransform(this->InputModelNodeToWorldTransform);

  this->PlaneClipper = vtkSmartPointer<vtkClipPolyData>::New();
  this->PlaneClipper->SetInputConnection(this->InputModelToWorldTransformFilter->GetOutputPort());

  this->Plane = vtkSmartPointer<vtkPlane>::New();
  this->PlaneClipper->SetClipFunction(this->Plane);
  this->PlaneClipper->GenerateClippedOutputOn();

  this->OutputPositiveModelToWorldTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->OutputPositiveWorldToModelTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  this->OutputPositiveModelToWorldTransformFilter->SetTransform(this->OutputPositiveWorldToModelTransform);
  this->OutputPositiveModelToWorldTransformFilter->SetInputConnection(this->PlaneClipper->GetOutputPort(0));

  this->OutputNegativeModelToWorldTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->OutputNegativeWorldToModelTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  this->OutputNegativeModelToWorldTransformFilter->SetTransform(this->OutputNegativeWorldToModelTransform);
  this->OutputNegativeModelToWorldTransformFilter->SetInputConnection(this->PlaneClipper->GetOutputPort(1));
}

//----------------------------------------------------------------------------
vtkSlicerPlaneCutRule::~vtkSlicerPlaneCutRule()
{
}

//----------------------------------------------------------------------------
const char* vtkSlicerPlaneCutRule::GetName()
{
  return "PlaneCut";
}

//----------------------------------------------------------------------------
bool vtkSlicerPlaneCutRule::RunInternal(vtkMRMLMeshModifyNode* meshModifyNode)
{
  if (!this->HasRequiredInputs(meshModifyNode))
    {
    vtkErrorMacro("Invalid number of inputs");
    return false;
    }

  vtkMRMLModelNode* outputPositiveModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthOutputNode(0, meshModifyNode)); // TODO: nth order is never defined
  vtkMRMLModelNode* outputNegativeModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthOutputNode(1, meshModifyNode)); // TODO: nth order is never defined
  if (!outputPositiveModelNode && !outputNegativeModelNode)
    {
    // Nothing to output
    return true;
    }

  vtkMRMLMarkupsPlaneNode* inputPlaneNode = vtkMRMLMarkupsPlaneNode::SafeDownCast(this->GetNthInputNode(0, meshModifyNode)); // TODO: nth order is never defined
  vtkMRMLModelNode* inputModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthInputNode(1, meshModifyNode)); // TODO: nth order is never defined
  if (!inputPlaneNode || !inputModelNode)
    {
    vtkErrorMacro("Invalid input nodes!");
    return false;
    }

  if (inputModelNode->GetParentTransformNode())
    {
    inputModelNode->GetParentTransformNode()->GetTransformToWorld(this->InputModelNodeToWorldTransform);
    }
  if (outputPositiveModelNode && outputPositiveModelNode->GetParentTransformNode())
    {
    outputPositiveModelNode->GetParentTransformNode()->GetTransformFromWorld(this->OutputPositiveWorldToModelTransform);
    }
  if (outputNegativeModelNode && outputNegativeModelNode->GetParentTransformNode())
    {
    outputNegativeModelNode->GetParentTransformNode()->GetTransformFromWorld(this->OutputNegativeWorldToModelTransform);
    }

  this->InputModelToWorldTransformFilter->SetInputConnection(inputModelNode->GetMeshConnection()); 

  double origin_World[3] = { 0.0 };
  inputPlaneNode->GetOriginWorld(origin_World);
  this->Plane->SetOrigin(origin_World);

  double normal_World[3] = { 0.0 };
  inputPlaneNode->GetNormalWorld(normal_World);
  this->Plane->SetNormal(normal_World);

  if (outputPositiveModelNode)
    {
    this->OutputPositiveModelToWorldTransformFilter->Update();
    outputPositiveModelNode->SetAndObserveMesh(this->OutputPositiveModelToWorldTransformFilter->GetOutput());
    }

  if (outputNegativeModelNode)
    {
    this->OutputNegativeModelToWorldTransformFilter->Update();
    outputNegativeModelNode->SetAndObserveMesh(this->OutputNegativeModelToWorldTransformFilter->GetOutput());
    }

  return true;
}