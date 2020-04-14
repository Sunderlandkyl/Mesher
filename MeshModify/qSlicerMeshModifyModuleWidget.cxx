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

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerMeshModifyModuleWidget.h"
#include "ui_qSlicerMeshModifyModuleWidget.h"

// MRML
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>


//
#include <vtkSlicerMeshModifyLogic.h>

// 
#include <vtkMRMLMeshModifyNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMeshModifyModuleWidgetPrivate: public Ui_qSlicerMeshModifyModuleWidget
{
public:
  qSlicerMeshModifyModuleWidgetPrivate();
  vtkMRMLMeshModifyNode* MeshModifyNode;

  QList<qMRMLNodeComboBox*> InputComboboxes;
  QList<qMRMLNodeComboBox*> OutputComboboxes;
};

//-----------------------------------------------------------------------------
// qSlicerMeshModifyModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerMeshModifyModuleWidgetPrivate::qSlicerMeshModifyModuleWidgetPrivate()
  : MeshModifyNode(nullptr)
{
}

//-----------------------------------------------------------------------------
// qSlicerMeshModifyModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerMeshModifyModuleWidget::qSlicerMeshModifyModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerMeshModifyModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerMeshModifyModuleWidget::~qSlicerMeshModifyModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerMeshModifyModuleWidget::setup()
{
  Q_D(qSlicerMeshModifyModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  connect(d->ParameterNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(onParameterNodeChanged(vtkMRMLNode*)));
}

//-----------------------------------------------------------------------------
void qSlicerMeshModifyModuleWidget::onParameterNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerMeshModifyModuleWidget);
  if (!this->mrmlScene() || this->mrmlScene()->IsBatchProcessing())
    {
    return;
    }


  vtkMRMLMeshModifyNode* meshModifyNode = vtkMRMLMeshModifyNode::SafeDownCast(node);
  qvtkReconnect(d->MeshModifyNode, meshModifyNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()));

  d->MeshModifyNode = meshModifyNode;
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerMeshModifyModuleWidget::updateInputSelectors()
{
  Q_D(qSlicerMeshModifyModuleWidget);
  vtkSlicerMeshModifyLogic* meshModifyLogic = vtkSlicerMeshModifyLogic::SafeDownCast(this->logic());
  vtkSlicerMeshModifyRule* rule = nullptr;
  if (meshModifyLogic && d->MeshModifyNode)
    {
    rule = meshModifyLogic->GetMeshModifyRule(d->MeshModifyNode);
    }

  QList<QWidget*> widgets = d->InputNodesCollapsibleButton->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
  for (QWidget* widget : widgets)
    {
    widget->deleteLater();
    }

  if (!rule)
    {
    return;
    }

  QWidget* inputNodesWidget = new QWidget();
  QFormLayout* inputNodesLayout = new QFormLayout();
  inputNodesWidget->setLayout(inputNodesLayout);
  d->InputNodesCollapsibleButton->layout()->addWidget(inputNodesWidget);
  for (int i = 0; i < rule->GetNumberOfInputNodes(); ++i)
    {
    std::string name = rule->GetNthInputNodeName(i);
    std::string description = rule->GetNthInputNodeDescription(i);
    std::string referenceRole = rule->GetNthInputNodeReferenceRole(i);
    std::string className = rule->GetNthInputNodeClassName(i);

    QLabel* nodeLabel = new QLabel();
    std::stringstream labelTextSS;
    labelTextSS << name << ":";
    std::string labelText = labelTextSS.str();
    nodeLabel->setText(labelText.c_str());
    nodeLabel->setToolTip(description.c_str());

    qMRMLNodeComboBox* nodeSelector = new qMRMLNodeComboBox();
    nodeSelector->setNodeTypes(QStringList() << className.c_str());
    nodeSelector->setToolTip(description.c_str());
    nodeSelector->setNoneEnabled(true);
    nodeSelector->setMRMLScene(this->mrmlScene());
    nodeSelector->setProperty("ReferenceRole", referenceRole.c_str());

    inputNodesLayout->addRow(nodeLabel, nodeSelector);

    connect(nodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
      this, SLOT(updateMRMLFromWidget()));
    }
}

//-----------------------------------------------------------------------------
void qSlicerMeshModifyModuleWidget::updateOutputSelectors()
{
  Q_D(qSlicerMeshModifyModuleWidget);
  vtkSlicerMeshModifyLogic* meshModifyLogic = vtkSlicerMeshModifyLogic::SafeDownCast(this->logic());
  vtkSlicerMeshModifyRule* rule = nullptr;
  if (meshModifyLogic && d->MeshModifyNode)
    {
    rule = meshModifyLogic->GetMeshModifyRule(d->MeshModifyNode);
    }

  QList<QWidget*> widgets = d->OutputNodesCollapsibleButton->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
  for (QWidget* widget : widgets)
    {
    widget->deleteLater();
    }

  if (!rule)
    {
    return;
    }

  QWidget* outputNodesWidget = new QWidget();
  QFormLayout* outputNodesLayout = new QFormLayout();
  outputNodesWidget->setLayout(outputNodesLayout);
  d->OutputNodesCollapsibleButton->layout()->addWidget(outputNodesWidget);
  for (int i = 0; i < rule->GetNumberOfOutputNodes(); ++i)
    {
    std::string name = rule->GetNthOutputNodeName(i);
    std::string description = rule->GetNthOutputNodeDescription(i);
    std::string referenceRole = rule->GetNthOutputNodeReferenceRole(i);
    std::string className = rule->GetNthOutputNodeClassName(i);

    QLabel* nodeLabel = new QLabel();
    std::stringstream labelTextSS;
    labelTextSS << name << ":";
    std::string labelText = labelTextSS.str();
    nodeLabel->setText(labelText.c_str());
    nodeLabel->setToolTip(description.c_str());

    qMRMLNodeComboBox* nodeSelector = new qMRMLNodeComboBox();
    nodeSelector->setNodeTypes(QStringList() << className.c_str());
    nodeSelector->setToolTip(description.c_str());
    nodeSelector->setNoneEnabled(true);
    nodeSelector->setMRMLScene(this->mrmlScene());
    nodeSelector->setProperty("ReferenceRole", referenceRole.c_str());

    outputNodesLayout->addRow(nodeLabel, nodeSelector);

    connect(nodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
      this, SLOT(updateMRMLFromWidget()));
    }
}

//-----------------------------------------------------------------------------
void qSlicerMeshModifyModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerMeshModifyModuleWidget);
  vtkSlicerMeshModifyLogic* meshModifyLogic = vtkSlicerMeshModifyLogic::SafeDownCast(this->logic());
  vtkSlicerMeshModifyRule* rule = nullptr;
  if (meshModifyLogic && d->MeshModifyNode)
    {
    rule = meshModifyLogic->GetMeshModifyRule(d->MeshModifyNode);
    }

  this->updateInputSelectors();
  this->updateOutputSelectors();

  if (!rule)
    {
    d->InputNodesCollapsibleButton->setEnabled(false);
    d->ParametersCollapsibleButton->setEnabled(false);
    d->OutputNodesCollapsibleButton->setEnabled(false);
    return;
    }

  d->InputNodesCollapsibleButton->setEnabled(true);
  d->ParametersCollapsibleButton->setEnabled(true);
  d->OutputNodesCollapsibleButton->setEnabled(true);

  QList< qMRMLNodeComboBox*> inputNodeSelectors = d->InputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* inputNodeSelector : inputNodeSelectors)
    {
    std::string referenceRole = inputNodeSelector->property("ReferenceRole").toString().toStdString();
    vtkMRMLNode* referenceNode = d->MeshModifyNode->GetNodeReference(referenceRole.c_str());
    bool wasBlocking = inputNodeSelector->blockSignals(true);
    inputNodeSelector->setCurrentNode(referenceNode);
    inputNodeSelector->blockSignals(wasBlocking);
    }

  QList< qMRMLNodeComboBox*> outputNodeSelectors = d->OutputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* outputNodeSelector : outputNodeSelectors)
    {
    std::string referenceRole = outputNodeSelector->property("ReferenceRole").toString().toStdString();
    vtkMRMLNode* referenceNode = d->MeshModifyNode->GetNodeReference(referenceRole.c_str());
    bool wasBlocking = outputNodeSelector->blockSignals(true);
    outputNodeSelector->setCurrentNode(referenceNode);
    outputNodeSelector->blockSignals(wasBlocking);
    }
}

//-----------------------------------------------------------------------------
void qSlicerMeshModifyModuleWidget::updateMRMLFromWidget()
{
  Q_D(qSlicerMeshModifyModuleWidget);
  if (!d->MeshModifyNode)
    {
    return;
    }

  MRMLNodeModifyBlocker blocker(d->MeshModifyNode);
  QList< qMRMLNodeComboBox*> inputNodeSelectors = d->InputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* inputNodeSelector : inputNodeSelectors)
    {
    std::string referenceRole = inputNodeSelector->property("ReferenceRole").toString().toStdString();
    QString currentNodeIDQString = inputNodeSelector->currentNodeID();
    std::string currentNodeIDString = currentNodeIDQString.toStdString();
    d->MeshModifyNode->SetNodeReferenceID(referenceRole.c_str(), currentNodeIDString.c_str());
    }

  QList< qMRMLNodeComboBox*> outputNodeSelectors = d->OutputNodesCollapsibleButton->findChildren<qMRMLNodeComboBox*>();
  for (qMRMLNodeComboBox* outputNodeSelector : outputNodeSelectors)
    {
    std::string referenceRole = outputNodeSelector->property("ReferenceRole").toString().toStdString();
    QString currentNodeIDQString = outputNodeSelector->currentNodeID();
    std::string currentNodeIDString = currentNodeIDQString.toStdString();
    d->MeshModifyNode->SetNodeReferenceID(referenceRole.c_str(), currentNodeIDString.c_str());
  }

}
