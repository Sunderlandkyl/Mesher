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

#ifndef __qSlicerMeshModifyModuleWidget_h
#define __qSlicerMeshModifyModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerMeshModifyModuleExport.h"

class qSlicerMeshModifyModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_MESHMODIFY_EXPORT qSlicerMeshModifyModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerMeshModifyModuleWidget(QWidget *parent=0);
  virtual ~qSlicerMeshModifyModuleWidget();

public slots:


protected:
  QScopedPointer<qSlicerMeshModifyModuleWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void updateInputSelectors();
  virtual void updateOutputSelectors();


protected slots:
  virtual void onParameterNodeChanged(vtkMRMLNode* node);
  virtual void updateWidgetFromMRML();
  virtual void updateMRMLFromWidget();
  virtual void onApplyButtonClicked();

private:
  Q_DECLARE_PRIVATE(qSlicerMeshModifyModuleWidget);
  Q_DISABLE_COPY(qSlicerMeshModifyModuleWidget);
};

#endif
