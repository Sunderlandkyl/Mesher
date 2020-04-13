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

#ifndef __vtkSlicerPlaneCutRule_h
#define __vtkSlicerPlaneCutRule_h

#include "vtkSlicerMeshModifyModuleLogicExport.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// STD includes
#include <map>
#include <string>
#include <vector>

class vtkClipPolyData;
class vtkDataObject;
class vtkGeneralTransform;
class vtkPlane;
class vtkTransformPolyDataFilter;

#include "vtkSlicerMeshModifyRule.h"

/// TODO
class VTK_SLICER_MESHMODIFY_MODULE_LOGIC_EXPORT vtkSlicerPlaneCutRule : public vtkSlicerMeshModifyRule
{
public:
  static vtkSlicerPlaneCutRule* New();
  virtual vtkSlicerMeshModifyRule* CreateRuleInstance() override;
  vtkTypeMacro(vtkSlicerPlaneCutRule, vtkSlicerMeshModifyRule);

  /// Human-readable name of the mesh modification rule
  virtual const char* GetName() override;

  virtual bool Run(vtkCollection* inputNodes, vtkCollection* outputNodes);

protected:
  vtkSlicerPlaneCutRule();
  ~vtkSlicerPlaneCutRule() override;
  void operator=(const vtkSlicerPlaneCutRule&);

protected:
  vtkSmartPointer<vtkTransformPolyDataFilter> InputModelToWorldTransformFilter;
  vtkSmartPointer<vtkGeneralTransform>        InputModelNodeToWorldTransform;

  vtkSmartPointer<vtkClipPolyData>            PlaneClipper;
  vtkSmartPointer<vtkPlane>                   Plane;

  vtkSmartPointer<vtkTransformPolyDataFilter> OutputModelToWorldTransformFilter;
  vtkSmartPointer<vtkGeneralTransform>        OutputWorldToModelTransform;
};

#endif // __vtkSlicerPlaneCutRule_h
