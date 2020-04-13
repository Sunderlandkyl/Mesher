/*==============================================================================

  Program: 3D Slicer

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

// Segmentations MRML includes
#include "vtkMRMLMeshModifyNode.h"

#include "vtkMRMLSegmentationNode.h"
#include "vtkOrientedImageDataResample.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>

//
#include <vtkMRMLMarkupsPlaneNode.h>
#include <vtkMRMLModelNode.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
static const char* INPUT_REFERENCE_ROLE = "inputNode";
static const char* INPUT_REFERENCE_ATTRIBUTE_NAME = "inputNodeRef";
static const char* OUTPUT_REFERENCE_ROLE = "outputNode";
static const char* OUTPUT_REFERENCE_ATTRIBUTE_NAME = "outputNodeRef";

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMeshModifyNode);

//----------------------------------------------------------------------------
vtkMRMLMeshModifyNode::vtkMRMLMeshModifyNode()
  : MethodName(nullptr)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextTuple1(vtkCommand::ModifiedEvent);
  events->InsertNextTuple1(vtkMRMLMarkupsPlaneNode::PointModifiedEvent); // TODO: Allow other events
  events->InsertNextTuple1(vtkMRMLModelNode::PolyDataModifiedEvent); // TODO: Allow other events
  this->AddNodeReferenceRole(INPUT_REFERENCE_ROLE, INPUT_REFERENCE_ATTRIBUTE_NAME, events);
  this->AddNodeReferenceRole(OUTPUT_REFERENCE_ROLE, OUTPUT_REFERENCE_ATTRIBUTE_NAME, nullptr);
}

//----------------------------------------------------------------------------
vtkMRMLMeshModifyNode::~vtkMRMLMeshModifyNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
  vtkMRMLWriteXMLBeginMacro(of);
  vtkMRMLWriteXMLStringMacro(methodName, MethodName);
  vtkMRMLWriteXMLBooleanMacro(continuousUpdate, ContinuousUpdate);
  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::ReadXMLAttributes(const char** atts)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::ReadXMLAttributes(atts);
  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLStringMacro(methodName, MethodName);
  vtkMRMLReadXMLBooleanMacro(continuousUpdate, ContinuousUpdate);
  vtkMRMLReadXMLEndMacro();
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLMeshModifyNode::Copy(vtkMRMLNode *anode)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::Copy(anode);
  vtkMRMLCopyBeginMacro(anode);
  vtkMRMLCopyStringMacro(MethodName);
  vtkMRMLCopyBooleanMacro(ContinuousUpdate);
  vtkMRMLCopyEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintStringMacro(MethodName);
  vtkMRMLPrintBooleanMacro(ContinuousUpdate);
  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::ProcessMRMLEvents(vtkObject* caller, unsigned long eventID, void* callData)
{
  Superclass::ProcessMRMLEvents(caller, eventID, callData);
  if (!this->Scene)
    {
    vtkErrorMacro("ProcessMRMLEvents: Invalid MRML scene");
    return;
    }

  vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);
  if (!node)
    {
    return;
    }

  std::vector<const char*> inputNodes;
  if (this->HasNodeReferenceID(INPUT_REFERENCE_ROLE, node->GetID()))
    {
    this->InvokeEvent(InputNodeModified, caller);
    }
}


//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::SetNthInputNode(int n, std::string id)
{
  this->SetNthNodeReferenceID(INPUT_REFERENCE_ROLE, n, id.c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::SetNthOutputNode(int n, std::string id)
{
  this->SetNthNodeReferenceID(OUTPUT_REFERENCE_ROLE, n, id.c_str());
}
