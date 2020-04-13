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

// MeshModify Logic includes
#include <vtkSlicerMeshModifyLogic.h>

// MeshModify includes
#include "qSlicerMeshModifyModule.h"
#include "qSlicerMeshModifyModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMeshModifyModulePrivate
{
public:
  qSlicerMeshModifyModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMeshModifyModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerMeshModifyModulePrivate::qSlicerMeshModifyModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerMeshModifyModule methods

//-----------------------------------------------------------------------------
qSlicerMeshModifyModule::qSlicerMeshModifyModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMeshModifyModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerMeshModifyModule::~qSlicerMeshModifyModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerMeshModifyModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerMeshModifyModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMeshModifyModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerMeshModifyModule::icon() const
{
  return QIcon(":/Icons/MeshModify.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerMeshModifyModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMeshModifyModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerMeshModifyModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerMeshModifyModule
::createWidgetRepresentation()
{
  return new qSlicerMeshModifyModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerMeshModifyModule::createLogic()
{
  return vtkSlicerMeshModifyLogic::New();
}
