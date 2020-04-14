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

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMeshModifyNode);

//----------------------------------------------------------------------------
vtkMRMLMeshModifyNode::vtkMRMLMeshModifyNode()
  : RuleName(nullptr)
{
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
  vtkMRMLWriteXMLStringMacro(methodName, RuleName);
  vtkMRMLWriteXMLBooleanMacro(continuousUpdate, ContinuousUpdate);
  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::ReadXMLAttributes(const char** atts)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::ReadXMLAttributes(atts);
  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLStringMacro(methodName, RuleName);
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
  vtkMRMLCopyStringMacro(RuleName);
  vtkMRMLCopyBooleanMacro(ContinuousUpdate);
  vtkMRMLCopyEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMeshModifyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintStringMacro(RuleName);
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
  this->InvokeEvent(InputNodeModified, caller);
}
