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

#ifndef __vtkSlicerMeshModifyRule_h
#define __vtkSlicerMeshModifyRule_h

#include "vtkSlicerMeshModifyModuleLogicExport.h"

// VTK includes
#include <vtkObject.h>

// STD includes
#include <map>
#include <string>
#include <vector>

class vtkCollection;

/// Helper macro for supporting cloning of rules
#ifndef vtkRuleNewMacro
#define vtkRuleNewMacro(newClass) \
vtkStandardNewMacro(newClass); \
vtkSlicerMeshModifyRule* newClass::CreateRuleInstance() \
{ \
return newClass::New(); \
}
#endif

/// TODO
class VTK_SLICER_MESHMODIFY_MODULE_LOGIC_EXPORT vtkSlicerMeshModifyRule : public vtkObject
{
public:
  vtkTypeMacro(vtkSlicerMeshModifyRule, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /// Create instance of the default node. Similar to New but virtual method.
  /// Subclasses should implement this method by
  virtual vtkSlicerMeshModifyRule* CreateRuleInstance() = 0;

  /// Create a new instance of this rule and copy its contents
  virtual vtkSlicerMeshModifyRule* Clone();

  /// Human-readable name of the mesh modification rule
  virtual const char* GetName() = 0;
  
  /// TODO
  int GetNumberOfInputNodes();
  int GetNumberOfOutputNodes();

  /// TODO
  std::string GetNthInputNodeName(int n);
  std::string GetNthInputNodeDescription(int n);
  std::string GetNthInputNodeClassName(int n);

  /// TODO
  std::string GetNthOutputNodeName(int n);
  std::string GetNthOutputNodeDescription(int n);
  std::string GetNthOutputNodeClassName(int n);

  virtual bool Run(vtkCollection* inputNodes, vtkCollection* outputNodes) = 0;

protected:
  vtkSlicerMeshModifyRule();
  ~vtkSlicerMeshModifyRule() override;
  void operator=(const vtkSlicerMeshModifyRule&);

protected:
  struct NodeInfo
  {
    std::string Name;
    std::string Description;
    std::string ClassName;
  };
  std::vector<NodeInfo> InputNodeInfo;
  std::vector<NodeInfo> OutputNodeInfo;

};

#endif // __vtkSlicerMeshModifyRule_h
