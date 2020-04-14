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

// .NAME vtkSlicerMeshModifyLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerMeshModifyLogic_h
#define __vtkSlicerMeshModifyLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// Logic includes
#include <vtkSlicerMeshModifyRule.h>

// STD includes
#include <cstdlib>

#include "vtkSlicerMeshModifyModuleLogicExport.h"

// VTK includes
#include <vtkSmartPointer.h>

class vtkMRMLMeshModifyNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_MESHMODIFY_MODULE_LOGIC_EXPORT vtkSlicerMeshModifyLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerMeshModifyLogic *New();
  vtkTypeMacro(vtkSlicerMeshModifyLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSlicerMeshModifyRule* GetMeshModifyRule(vtkMRMLMeshModifyNode* meshModifyNode);
  void RunMeshModifyRule(vtkMRMLMeshModifyNode* meshModifyNode);

protected:
  vtkSlicerMeshModifyLogic();
  virtual ~vtkSlicerMeshModifyLogic();
  void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) override;

  void SetMRMLSceneInternal(vtkMRMLScene* newScene) override;
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  void RegisterNodes() override;
  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;

  void UpdateMeshModifyRule(vtkMRMLMeshModifyNode* meshModifyNode);

private:

  typedef std::map<std::string,vtkSmartPointer<vtkSlicerMeshModifyRule> > MeshModifyRuleList;
  MeshModifyRuleList Rules;

  vtkSlicerMeshModifyLogic(const vtkSlicerMeshModifyLogic&); // Not implemented
  void operator=(const vtkSlicerMeshModifyLogic&); // Not implemented
};

#endif
