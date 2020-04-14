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

#include "vtkSlicerMeshModifyRule.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

///
#include "vtkMRMLMeshModifyNode.h"

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRule::vtkSlicerMeshModifyRule()
{
}

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRule::~vtkSlicerMeshModifyRule()
{
}

//----------------------------------------------------------------------------
vtkSlicerMeshModifyRule* vtkSlicerMeshModifyRule::Clone()
{
  vtkSlicerMeshModifyRule* clone = this->CreateRuleInstance();
  return clone;
}

//----------------------------------------------------------------------------
int vtkSlicerMeshModifyRule::GetNumberOfInputNodes()
{
  return this->InputNodeInfo.size();
}

//----------------------------------------------------------------------------
int vtkSlicerMeshModifyRule::GetNumberOfOutputNodes()
{
  return this->OutputNodeInfo.size();
}

//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthInputNodeName(int n)
{
  if (n > this->InputNodeInfo.size())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return "";
    }
  return this->InputNodeInfo[n].Name;
}

//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthInputNodeDescription(int n)
{
  if (n > this->InputNodeInfo.size())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return "";
    }
  return this->InputNodeInfo[n].Description;
}

//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthInputNodeClassName(int n)
{
  if (n > this->InputNodeInfo.size())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return "";
    }
  return this->InputNodeInfo[n].ClassName;
}

//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthInputNodeReferenceRole(int n)
{
  if (n > this->InputNodeInfo.size())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return "";
    }
  return this->InputNodeInfo[n].ReferenceRole;
}

//----------------------------------------------------------------------------
bool vtkSlicerMeshModifyRule::GetNthInputNodeRequired(int n)
{
  if (n > this->InputNodeInfo.size())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return false;
    }
  return this->InputNodeInfo[n].Required;
}


//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthOutputNodeName(int n)
{
  if (n > this->OutputNodeInfo.size())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return "";
    }
  return this->OutputNodeInfo[n].Name;
}

//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthOutputNodeDescription(int n)
{
  if (n > this->OutputNodeInfo.size())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return "";
    }
  return this->OutputNodeInfo[n].Description;
}

//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthOutputNodeClassName(int n)
{
  if (n > this->OutputNodeInfo.size())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return "";
    }
  return this->OutputNodeInfo[n].ClassName;
}

//----------------------------------------------------------------------------
std::string vtkSlicerMeshModifyRule::GetNthOutputNodeReferenceRole(int n)
{
  if (n > this->OutputNodeInfo.size())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return "";
    }
  return this->OutputNodeInfo[n].ReferenceRole;
}

//----------------------------------------------------------------------------
bool vtkSlicerMeshModifyRule::GetNthOutputNodeRequired(int n)
{
  if (n > this->OutputNodeInfo.size())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return false;
    }
  return this->OutputNodeInfo[n].Required;
}

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyRule::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Name:\t" << this->GetName() << std::endl;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkSlicerMeshModifyRule::GetNthInputNodeEvents(int n)
{
  if (n > this->InputNodeInfo.size())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return false;
    }
  return this->InputNodeInfo[n].Events;
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkSlicerMeshModifyRule::GetNthInputNode(int n, vtkMRMLMeshModifyNode* meshModifyNode)
{
  if(!meshModifyNode)
    {
    vtkErrorMacro("Invalid parameter node");
    return nullptr;
    }
  if (n > this->InputNodeInfo.size())
    {
    vtkErrorMacro("Input node " << n << " is out of range!");
    return false;
    }
  std::string referenceRole = this->GetNthInputNodeReferenceRole(n);
  return meshModifyNode->GetNodeReference(referenceRole.c_str());
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkSlicerMeshModifyRule::GetNthOutputNode(int n, vtkMRMLMeshModifyNode* meshModifyNode)
{
  if(!meshModifyNode)
    {
    vtkErrorMacro("Invalid parameter node");
    return nullptr;
    }
  if (n > this->OutputNodeInfo.size())
    {
    vtkErrorMacro("Output node " << n << " is out of range!");
    return false;
    }
  std::string referenceRole = this->GetNthOutputNodeReferenceRole(n);
  return meshModifyNode->GetNodeReference(referenceRole.c_str());
}

//---------------------------------------------------------------------------
bool vtkSlicerMeshModifyRule::HasRequiredInputs(vtkMRMLMeshModifyNode* meshModifyNode)
{
  for (int i = 0; i < this->GetNumberOfInputNodes(); ++i)
    {
    if (!this->GetNthInputNodeRequired(i))
      {
      continue;
      }

    std::string referenceRole = this->GetNthInputNodeReferenceRole(i);
    if (meshModifyNode->GetNodeReference(referenceRole.c_str()) == nullptr)
      {
      return false;
      }
    }
  return true;
}

//---------------------------------------------------------------------------
bool vtkSlicerMeshModifyRule::Run(vtkMRMLMeshModifyNode* meshModifyNode)
{
  if (!this->HasRequiredInputs(meshModifyNode))
    {
    vtkErrorMacro("Input node missing!");
    return false;
    }
  return this->RunInternal(meshModifyNode);
}