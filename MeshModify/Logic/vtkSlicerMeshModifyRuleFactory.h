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

#ifndef __vtkSlicerMeshModifyRuleFactory_h
#define __vtkSlicerMeshModifyRuleFactory_h

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// STD includes
#include <vector>

#include "vtkSlicerMeshModifyModuleLogicExport.h"

#include "vtkSlicerMeshModifyRule.h"

class vtkDataObject;

/// \ingroup SegmentationCore
/// \brief Class that can create vtkSegmentationConverter instances.
///
/// This singleton class is a repository of all segmentation converter rules.
/// Singleton pattern adopted from vtkEventBroker class.
class VTK_SLICER_MESHMODIFY_MODULE_LOGIC_EXPORT vtkSlicerMeshModifyRuleFactory : public vtkObject
{
public:

  vtkTypeMacro(vtkSlicerMeshModifyRuleFactory, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /// Registers a new mesh modify rule
  /// Returns true if the codec is successfully registered
  bool RegisterMeshModifyRule(vtkSmartPointer<vtkSlicerMeshModifyRule> rule);

  /// Removes a mesh modify rule from the factory
  /// This does not affect codecs that have already been instantiated
  /// Returns true if the codec is successfully unregistered
  bool UnregisterMeshModifyRuleByClassName(const std::string& ruleClassName);

  /// Get pointer to codec new rule, or nullptr if the rule is not registered
  /// Returns nullptr if no matching rule can be found
  vtkSlicerMeshModifyRule* CreateRuleByClassName(const std::string& codecClassName);

  /// Get pointer to codec new rule, or nullptr if the rule is not registered
  /// Returns nullptr if no matching rule can be found
  vtkSlicerMeshModifyRule* CreateRuleByName(const std::string name);

  /// Returns a list of all registered Codecs
  const std::vector<std::string> GetMeshModifyRuleClassNames();

  /// Returns a list of all registered Codecs
  const std::vector<std::string> GetMeshModifyRuleNames();

public:
  /// Return the singleton instance with no reference counting.
  static vtkSlicerMeshModifyRuleFactory* GetInstance();

  /// This is a singleton pattern New.  There will only be ONE
  /// reference to a vtkSlicerMeshModifyRuleFactory object per process.  Clients that
  /// call this must call Delete on the object so that the reference
  /// counting will work. The single instance will be unreferenced when
  /// the program exits.
  static vtkSlicerMeshModifyRuleFactory* New();

protected:
  vtkSlicerMeshModifyRuleFactory();
  ~vtkSlicerMeshModifyRuleFactory() override;
  vtkSlicerMeshModifyRuleFactory(const vtkSlicerMeshModifyRuleFactory&);
  void operator=(const vtkSlicerMeshModifyRuleFactory&);

  friend class vtkSlicerMeshModifyRuleFactoryInitialize;
  typedef vtkSlicerMeshModifyRuleFactory Self;

  // Singleton management functions.
  static void classInitialize();
  static void classFinalize();

  /// Registered rule classes
  std::vector< vtkSmartPointer<vtkSlicerMeshModifyRule> > RegisteredRules;
};


/// Utility class to make sure qSlicerModuleManager is initialized before it is used.
class VTK_SLICER_MESHMODIFY_MODULE_LOGIC_EXPORT vtkSlicerMeshModifyRuleFactoryInitialize
{
public:
  typedef vtkSlicerMeshModifyRuleFactoryInitialize Self;

  vtkSlicerMeshModifyRuleFactoryInitialize();
  ~vtkSlicerMeshModifyRuleFactoryInitialize();
private:
  static unsigned int Count;
};

/// This instance will show up in any translation unit that uses
/// vtkSlicerMeshModifyRuleFactory.  It will make sure vtkSlicerMeshModifyRuleFactory is initialized
/// before it is used.
static vtkSlicerMeshModifyRuleFactoryInitialize vtkSlicerMeshModifyRuleFactoryInitializer;

#endif
