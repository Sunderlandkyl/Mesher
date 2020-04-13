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

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

#include "vtkSlicerPlaneCutRule.h"

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
  NodeInfo input0Info;
  input0Info.Name = "Plane node";
  input0Info.Description = "Plane node to cut the model node.";
  input0Info.ClassName = "vtkMRMLMarkupsPlaneNode";

  NodeInfo input1Info;
  input1Info.Name = "Model node";
  input1Info.Description = "Model node to be cut.";
  input1Info.ClassName = "vtkMRMLModelNode";

  this->InputNodeInfo.push_back(input0Info);
  this->InputNodeInfo.push_back(input1Info);

  NodeInfo outputInfo;
  outputInfo.Name = "Model node";
  outputInfo.Description = "Cut model node.";
  outputInfo.ClassName = "vtkMRMLModelNode";

  this->OutputNodeInfo.push_back(outputInfo);

  this->InputModelToWorldTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->InputModelNodeToWorldTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  this->InputModelToWorldTransformFilter->SetTransform(this->InputModelNodeToWorldTransform);

  this->PlaneClipper = vtkSmartPointer<vtkClipPolyData>::New();
  this->PlaneClipper->SetInputConnection(this->InputModelToWorldTransformFilter->GetOutputPort());

  this->Plane = vtkSmartPointer<vtkPlane>::New();
  this->PlaneClipper->SetClipFunction(this->Plane);

  this->OutputModelToWorldTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->OutputWorldToModelTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  this->OutputModelToWorldTransformFilter->SetTransform(this->OutputWorldToModelTransform); 
  this->OutputModelToWorldTransformFilter->SetInputConnection(this->PlaneClipper->GetOutputPort());
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
bool vtkSlicerPlaneCutRule::Run(vtkCollection* inputNodes, vtkCollection* outputNodes)
{
  if (inputNodes->GetNumberOfItems() != this->GetNumberOfInputNodes())
    {
    vtkErrorMacro("Invalid number of inputs");
    return false;
    }
  if (outputNodes->GetNumberOfItems() != this->GetNumberOfOutputNodes())
    {
    vtkErrorMacro("Invalid number of outputs");
    return false;
    }

  vtkMRMLMarkupsPlaneNode* inputPlaneNode = vtkMRMLMarkupsPlaneNode::SafeDownCast(inputNodes->GetItemAsObject(0));
  vtkMRMLModelNode* inputModelNode = vtkMRMLModelNode::SafeDownCast(inputNodes->GetItemAsObject(1));
  vtkMRMLModelNode* outputModelNode = vtkMRMLModelNode::SafeDownCast(outputNodes->GetItemAsObject(0));
  if (!inputPlaneNode || !inputModelNode || !outputModelNode)
    {
    vtkErrorMacro("Invalid input!");
    return false;
    }

  if (inputModelNode->GetParentTransformNode())
    {
    inputModelNode->GetParentTransformNode()->GetTransformToWorld(this->InputModelNodeToWorldTransform);
    }
  if (outputModelNode->GetParentTransformNode())
    {
    outputModelNode->GetParentTransformNode()->GetTransformFromWorld(this->OutputWorldToModelTransform);
    }

  this->InputModelToWorldTransformFilter->SetInputConnection(inputModelNode->GetMeshConnection());

  double origin_World[3] = { 0.0 };
  inputPlaneNode->GetOriginWorld(origin_World);
  this->Plane->SetOrigin(origin_World);

  double normal_World[3] = { 0.0 };
  inputPlaneNode->GetNormalWorld(normal_World);
  this->Plane->SetNormal(normal_World);

  this->OutputModelToWorldTransformFilter->Update();
  outputModelNode->SetAndObserveMesh(this->OutputModelToWorldTransformFilter->GetOutput());

  return true;
}