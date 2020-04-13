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

#ifndef __vtkMRMLMeshModifyNode_h
#define __vtkMRMLMeshModifyNode_h

// MRML includes
#include <vtkMRMLNode.h>

// Segmentations includes
#include "vtkSlicerSegmentationsModuleMRMLExport.h"

#include "vtkOrientedImageData.h"

class vtkMRMLScene;
class vtkMRMLScalarVolumeNode;
class vtkMRMLSegmentationNode;

/// \ingroup Segmentations
/// \brief Parameter set node for the segment editor widget
///
/// Stores parameters for a segment editor widget (selected segmentation, segment, master volume),
/// and all the editor effects. The effect parameters are stored as attributes with names
/// EffectName.ParameterName. If a parameter is changed, the node Modified event is not emitted,
/// but the custom EffectParameterModified event that triggers update of the effect options widget only.
///
class VTK_SLICER_SEGMENTATIONS_MODULE_MRML_EXPORT vtkMRMLMeshModifyNode : public vtkMRMLNode
{
public:
  static vtkMRMLMeshModifyNode *New();
  vtkTypeMacro(vtkMRMLMeshModifyNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /// Create instance of a MeshModify node.
  vtkMRMLNode* CreateNodeInstance() override;

  /// Set node attributes from name/value pairs
  void ReadXMLAttributes( const char** atts) override;

  /// Write this node's information to a MRML file in XML format.
  void WriteXML(ostream& of, int indent) override;

  /// Copy the node's attributes to this object
  void Copy(vtkMRMLNode *node) override;

  /// Get unique node XML tag name (like Volume, Model)
  const char* GetNodeTagName() override { return "MeshModify"; }

public:

  vtkGetStringMacro(MethodName);
  vtkSetStringMacro(MethodName); 

protected:
  vtkMRMLMeshModifyNode();
  ~vtkMRMLMeshModifyNode() override;
  vtkMRMLMeshModifyNode(const vtkMRMLMeshModifyNode&);
  void operator=(const vtkMRMLMeshModifyNode&);

  /// Selected segment ID
  char* MethodName;
};

#endif // __vtkMRMLMeshModifyNode_h
