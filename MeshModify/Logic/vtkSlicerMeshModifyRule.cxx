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

#include "vtkSlicerMeshModifyRule.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>


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

//---------------------------------------------------------------------------
void vtkSlicerMeshModifyRule::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Name:\t" << this->GetName() << std::endl;
}