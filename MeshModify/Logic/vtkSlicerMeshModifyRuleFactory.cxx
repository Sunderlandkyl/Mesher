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

// MeshModify includes
#include "vtkSlicerMeshModifyRuleFactory.h"
#include "vtkSlicerMeshModifyRule.h"
#include "vtkSlicerFreeSurferParcellateRule.h"
#include "vtkSlicerPlaneCutRule.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkDataObject.h>

// STD includes
#include <algorithm>

//----------------------------------------------------------------------------
// The compression rule manager singleton.
// This MUST be default initialized to zero by the compiler and is
// therefore not initialized here.  The ClassInitialize and ClassFinalize methods handle this instance.
static vtkSlicerMeshModifyRuleFactory* vtkSlicerMeshModifyRuleFactoryInstance;


//----------------------------------------------------------------------------
// Must NOT be initialized.  Default initialization to zero is necessary.
unsigned int vtkSlicerMeshModifyRuleFactoryInitialize::Count;

//----------------------------------------------------------------------------
// Implementation of vtkSlicerMeshModifyRuleFactoryInitialize class.
//----------------------------------------------------------------------------
vtkSlicerMeshModifyRuleFactoryInitialize::vtkSlicerMeshModifyRuleFactoryInitialize()
{
  if (++Self::Count == 1)
  {
    vtkSlicerMeshModifyRuleFactory::classInitialize();
  }
}

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRuleFactoryInitialize::~vtkSlicerMeshModifyRuleFactoryInitialize()
{
  if (--Self::Count == 0)
  {
    vtkSlicerMeshModifyRuleFactory::classFinalize();
  }
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkSlicerMeshModifyRuleFactory* vtkSlicerMeshModifyRuleFactory::New()
{
  vtkSlicerMeshModifyRuleFactory* ret = vtkSlicerMeshModifyRuleFactory::GetInstance();
  ret->Register(nullptr);
  return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkSlicerMeshModifyRuleFactory
vtkSlicerMeshModifyRuleFactory* vtkSlicerMeshModifyRuleFactory::GetInstance()
{
  if (!vtkSlicerMeshModifyRuleFactoryInstance)
  {
    // Try the factory first
    vtkSlicerMeshModifyRuleFactoryInstance = (vtkSlicerMeshModifyRuleFactory*)vtkObjectFactory::CreateInstance("vtkSlicerMeshModifyRuleFactory");
    // if the factory did not provide one, then create it here
    if (!vtkSlicerMeshModifyRuleFactoryInstance)
    {
      vtkSlicerMeshModifyRuleFactoryInstance = new vtkSlicerMeshModifyRuleFactory;
#ifdef VTK_HAS_INITIALIZE_OBJECT_BASE
      vtkSlicerMeshModifyRuleFactoryInstance->InitializeObjectBase();
#endif
    }
  }
  // return the instance
  return vtkSlicerMeshModifyRuleFactoryInstance;
}

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRuleFactory::vtkSlicerMeshModifyRuleFactory()
= default;

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRuleFactory::~vtkSlicerMeshModifyRuleFactory()
= default;

//----------------------------------------------------------------------------
void vtkSlicerMeshModifyRuleFactory::PrintSelf(ostream & os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerMeshModifyRuleFactory::classInitialize()
{
  // Allocate the singleton
  vtkSlicerMeshModifyRuleFactoryInstance = vtkSlicerMeshModifyRuleFactory::GetInstance();

  vtkSlicerMeshModifyRuleFactoryInstance->RegisterMeshModifyRule(vtkSmartPointer<vtkSlicerPlaneCutRule>::New());
  vtkSlicerMeshModifyRuleFactoryInstance->RegisterMeshModifyRule(vtkSmartPointer<vtkSlicerFreeSurferParcellateRule>::New());
}

//----------------------------------------------------------------------------
void vtkSlicerMeshModifyRuleFactory::classFinalize()
{
  vtkSlicerMeshModifyRuleFactoryInstance->Delete();
  vtkSlicerMeshModifyRuleFactoryInstance = nullptr;
}

//----------------------------------------------------------------------------
bool vtkSlicerMeshModifyRuleFactory::RegisterMeshModifyRule(vtkSmartPointer<vtkSlicerMeshModifyRule> rule)
{
  for (unsigned int i = 0; i < this->RegisteredRules.size(); ++i)
  {
    if (strcmp(this->RegisteredRules[i]->GetClassName(), rule->GetClassName()) == 0)
    {
      vtkWarningMacro("RegisterStreamingCodec failed: rule is already registered");
      return false;
    }
  }
  this->RegisteredRules.push_back(rule);
  return true;
}

//----------------------------------------------------------------------------
bool vtkSlicerMeshModifyRuleFactory::UnregisterMeshModifyRuleByClassName(const std::string & className)
{
  std::vector<vtkSmartPointer<vtkSlicerMeshModifyRule> >::iterator ruleIt;
  for (ruleIt = this->RegisteredRules.begin(); ruleIt != this->RegisteredRules.end(); ++ruleIt)
  {
    vtkSmartPointer<vtkSlicerMeshModifyRule> rule = *ruleIt;
    if (strcmp(rule->GetClassName(), className.c_str()) == 0)
    {
      this->RegisteredRules.erase(ruleIt);
      return true;
    }
  }
  vtkWarningMacro("UnRegisterStreamingCodecByClassName failed: rule not found");
  return false;
}

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRule* vtkSlicerMeshModifyRuleFactory::CreateRuleByClassName(const std::string & className)
{
  std::vector<vtkSmartPointer<vtkSlicerMeshModifyRule> >::iterator ruleIt;
  for (ruleIt = this->RegisteredRules.begin(); ruleIt != this->RegisteredRules.end(); ++ruleIt)
  {
    vtkSmartPointer<vtkSlicerMeshModifyRule> rule = *ruleIt;
    if (strcmp(rule->GetClassName(), className.c_str()) == 0)
    {
      return rule->CreateRuleInstance();
    }
  }
  return nullptr;
}

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRule* vtkSlicerMeshModifyRuleFactory::CreateRuleByName(const std::string name)
{
  std::vector<vtkSmartPointer<vtkSlicerMeshModifyRule> >::iterator ruleIt;
  for (ruleIt = this->RegisteredRules.begin(); ruleIt != this->RegisteredRules.end(); ++ruleIt)
  {
    vtkSmartPointer<vtkSlicerMeshModifyRule> rule = *ruleIt;
    if (rule->GetName() == name)
    {
      return rule->CreateRuleInstance();
    }
  }
  return nullptr;
}

//----------------------------------------------------------------------------
const std::vector<std::string> vtkSlicerMeshModifyRuleFactory::GetMeshModifyRuleClassNames()
{
  std::vector<std::string> ruleClassNames;
  std::vector<vtkSmartPointer<vtkSlicerMeshModifyRule> >::iterator ruleIt;
  for (ruleIt = this->RegisteredRules.begin(); ruleIt != this->RegisteredRules.end(); ++ruleIt)
  {
    vtkSmartPointer<vtkSlicerMeshModifyRule> rule = *ruleIt;
    ruleClassNames.emplace_back(rule->GetClassName());
  }
  return ruleClassNames;
}

//----------------------------------------------------------------------------
const std::vector<std::string> vtkSlicerMeshModifyRuleFactory::GetMeshModifyRuleNames()
{
  std::vector<std::string> names;
  std::vector<vtkSmartPointer<vtkSlicerMeshModifyRule> >::iterator ruleIt;
  for (ruleIt = this->RegisteredRules.begin(); ruleIt != this->RegisteredRules.end(); ++ruleIt)
  {
    vtkSmartPointer<vtkSlicerMeshModifyRule> rule = *ruleIt;
    names.push_back(rule->GetName());
  }
  return names;
}
