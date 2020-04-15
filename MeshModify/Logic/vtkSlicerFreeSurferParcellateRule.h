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

#ifndef __vtkSlicerFreeSurferParcellateRule_h
#define __vtkSlicerFreeSurferParcellateRule_h

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
class vtkMRMLMeshModifyNode;
class vtkPlane;
class vtkPolyData;
class vtkTransformPolyDataFilter;

#include "vtkSlicerMeshModifyRule.h"

/// TODO
class VTK_SLICER_MESHMODIFY_MODULE_LOGIC_EXPORT vtkSlicerFreeSurferParcellateRule : public vtkSlicerMeshModifyRule
{
public:
  static vtkSlicerFreeSurferParcellateRule* New();
  virtual vtkSlicerMeshModifyRule* CreateRuleInstance() override;
  vtkTypeMacro(vtkSlicerFreeSurferParcellateRule, vtkSlicerMeshModifyRule);

  /// Human-readable name of the mesh modification rule
  virtual const char* GetName() override;

  virtual bool RunInternal(vtkMRMLMeshModifyNode* meshModifyNode) override;

protected:
  vtkSlicerFreeSurferParcellateRule();
  ~vtkSlicerFreeSurferParcellateRule() override;
  void operator=(const vtkSlicerFreeSurferParcellateRule&);

protected:
  //TODO

  void ProjectCurveToSurface(vtkPoints* curve, vtkPolyData* originalSurface, vtkPolyData* projectedSurface, vtkPoints* outputProjectedPoints);
  void ClipCurveWithPlane(vtkPoints* inputCurve, vtkPlane* clipPlane, vtkPoints* outputCurve);
  void CombineCurves(vtkPoints* inputCurve1, vtkPoints* inputCurve2, vtkPolyData* inputModel, vtkPoints* outputClosedCurve);
  void SetCurveClockwise(vtkPoints* curve);
  void GetSurfacePatch(vtkPoints* inputClosedCurve, vtkPolyData* inputMesh, vtkPolyData* outputPatch);
  void StitchEdges(vtkPolyData* inputPolyData, vtkPoints* inputCurve1, vtkPoints* inputCurve2, vtkPolyData* outputPolyData);
};

#endif // __vtkSlicerFreeSurferParcellateRule_h
