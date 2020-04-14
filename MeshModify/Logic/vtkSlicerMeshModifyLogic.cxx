/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// MeshModify Logic includes
#include "vtkSlicerMeshModifyLogic.h"
#include "vtkSlicerMeshModifyRuleFactory.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLMeshModifyNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerMeshModifyLogic);

//----------------------------------------------------------------------------
vtkSlicerMeshModifyLogic::vtkSlicerMeshModifyLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerMeshModifyLogic::~vtkSlicerMeshModifyLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic::RegisterNodes()
{
  if (this->GetMRMLScene() == NULL)
    {
    vtkErrorMacro("Scene is invalid");
    return;
    }
  this->GetMRMLScene()->RegisterNodeClass(vtkSmartPointer<vtkMRMLMeshModifyNode>::New());
}

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  vtkMRMLMeshModifyNode* meshModifyNode = vtkMRMLMeshModifyNode::SafeDownCast(node);
  if (!meshModifyNode)
    {
    return;
    }

  this->Rules[meshModifyNode->GetID()] = nullptr;
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  events->InsertNextValue(vtkMRMLMeshModifyNode::InputNodeModified);
  vtkObserveMRMLNodeEventsMacro(meshModifyNode, events);
  this->UpdateMeshModifyRule(meshModifyNode);
  this->RunMeshModifyRule(meshModifyNode);
}

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  vtkMRMLMeshModifyNode* meshModifyNode = vtkMRMLMeshModifyNode::SafeDownCast(node);
  if (!meshModifyNode)
    {
    return;
    }

  MeshModifyRuleList::iterator rule = this->Rules.find(meshModifyNode->GetID());
  if (rule == this->Rules.end())
    {
    return;
    }
  this->Rules.erase(rule);
}

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic::ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData)
{
  Superclass::ProcessMRMLNodesEvents(caller, event, callData);
  vtkMRMLMeshModifyNode* meshModifyNode = vtkMRMLMeshModifyNode::SafeDownCast(caller);
  if (!meshModifyNode)
    {
    return;
    }

  if (event == vtkCommand::ModifiedEvent)
    {
    this->UpdateMeshModifyRule(meshModifyNode);
    }

  if (meshModifyNode && event == vtkMRMLMeshModifyNode::InputNodeModified && meshModifyNode->GetContinuousUpdate())
    {
    this->RunMeshModifyRule(meshModifyNode);
    }
}

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic::UpdateMeshModifyRule(vtkMRMLMeshModifyNode* meshModifyNode)
{
  vtkSmartPointer<vtkSlicerMeshModifyRule> rule = this->GetMeshModifyRule(meshModifyNode);
  if (!rule || strcmp(rule->GetName(), meshModifyNode->GetRuleName()) != 0)
    {
    if (meshModifyNode->GetRuleName())
      {
      rule = vtkSmartPointer<vtkSlicerMeshModifyRule>::Take(
        vtkSlicerMeshModifyRuleFactory::GetInstance()->CreateRuleByName(meshModifyNode->GetRuleName()));
      this->Rules[meshModifyNode->GetID()] = rule;
      }
    }
}

//---------------------------------------------------------------------------
vtkSlicerMeshModifyRule* vtkSlicerMeshModifyLogic::GetMeshModifyRule(vtkMRMLMeshModifyNode* meshModifyNode)
{
  vtkSmartPointer<vtkSlicerMeshModifyRule> rule = nullptr;
  MeshModifyRuleList::iterator ruleIt = this->Rules.find(meshModifyNode->GetID());
  if (ruleIt == this->Rules.end())
    {
    return nullptr;
    }
  return ruleIt->second;
}

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyLogic::RunMeshModifyRule(vtkMRMLMeshModifyNode* meshModifyNode)
{
  if (!meshModifyNode)
    {
    vtkErrorMacro("Invalid parameter node!");
    return;
    }
  if (!meshModifyNode->GetRuleName())
    {
    return;
    }

  vtkSmartPointer<vtkSlicerMeshModifyRule> rule = this->GetMeshModifyRule(meshModifyNode);
  if (!rule)
    {
    vtkErrorMacro("Could not find rule with name: " << meshModifyNode->GetRuleName());
    return;
    }
  if (!rule->HasRequiredInputs(meshModifyNode))
    {
    return;
    }

  rule->Run(meshModifyNode);
}
