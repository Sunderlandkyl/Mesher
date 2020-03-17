import os
import unittest
import logging
import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
from slicer.util import VTKObservationMixin

#
# Mesher2
#

class Mesher2(ScriptedLoadableModule):
  """Uses ScriptedLoadableModule base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "Mesher2"  # TODO: make this more human readable by adding spaces
    self.parent.categories = ["Examples"]  # TODO: set categories (folders where the module shows up in the module selector)
    self.parent.dependencies = []  # TODO: add here list of module names that this module requires
    self.parent.contributors = ["John Doe (AnyWare Corp.)"]  # TODO: replace with "Firstname Lastname (Organization)"
    self.parent.helpText = """
This is an example of scripted loadable module bundled in an extension.
It performs a simple thresholding on the input volume and optionally captures a screenshot.
"""  # TODO: update with short description of the module
    self.parent.helpText += self.getDefaultModuleDocumentationLink()  # TODO: verify that the default URL is correct or change it to the actual documentation
    self.parent.acknowledgementText = """
This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
and Steve Pieper, Isomics, Inc. and was partially funded by NIH grant 3P41RR013218-12S1.
"""  # TODO: replace with organization, grant and thanks.

#
# Mesher2Widget
#

class Mesher2Widget(ScriptedLoadableModuleWidget, VTKObservationMixin):
  """Uses ScriptedLoadableModuleWidget base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def __init__(self, parent=None):
    ScriptedLoadableModuleWidget.__init__(self, parent)
    VTKObservationMixin.__init__(self)  # needed for parameter node observation
    self.logic = None
    self._parameterNode = None

  def setup(self):
    """
    Called when the user opens the module the first time and the widget is initialized.
    """
    ScriptedLoadableModuleWidget.setup(self)

    # Load widget from .ui file (created by Qt Designer)
    uiWidget = slicer.util.loadUI(self.resourcePath('UI/Mesher2.ui'))
    self.layout.addWidget(uiWidget)
    self.ui = slicer.util.childWidgetVariables(uiWidget)

    # Set scene in MRML widgets. Make sure that in Qt designer
    # "mrmlSceneChanged(vtkMRMLScene*)" signal in is connected to each MRML widget's.
    # "setMRMLScene(vtkMRMLScene*)" slot.
    uiWidget.setMRMLScene(slicer.mrmlScene)

    # Create a new parameterNode (it stores user's node and parameter values choices in the scene)
    self.logic = Mesher2Logic()
    self.ui.parameterNodeSelector.addAttribute("vtkMRMLScriptedModuleNode", "ModuleName", self.moduleName)
    self.setParameterNode(self.logic.getParameterNode())

    # Connections
    self.ui.parameterNodeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.setParameterNode)

    self.ui.inputModelSelector1.connect("currentNodeChanged(vtkMRMLNode*)", self.updateParameterNodeFromGUI)
    self.ui.inputModelSelector2.connect("currentNodeChanged(vtkMRMLNode*)", self.updateParameterNodeFromGUI)
    self.ui.inputCurveSelector1.connect("currentNodeChanged(vtkMRMLNode*)", self.updateParameterNodeFromGUI)
    self.ui.inputCurveSelector2.connect("currentNodeChanged(vtkMRMLNode*)", self.updateParameterNodeFromGUI)
    self.ui.inputPlaneSelector1.connect("currentNodeChanged(vtkMRMLNode*)", self.updateParameterNodeFromGUI)
    self.ui.inputPlaneSelector2.connect("currentNodeChanged(vtkMRMLNode*)", self.updateParameterNodeFromGUI)
    self.ui.outputModelSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.updateParameterNodeFromGUI)

    self.ui.applyButton.connect('clicked(bool)', self.onApplyButton)

    # Initial GUI update
    self.updateGUIFromParameterNode()

  def cleanup(self):
    """
    Called when the application closes and the module widget is destroyed.
    """
    self.removeObservers()

  def setParameterNode(self, inputParameterNode):
    """
    Adds observers to the selected parameter node. Observation is needed because when the
    parameter node is changed then the GUI must be updated immediately.
    """
    wasBlocked = self.ui.parameterNodeSelector.blockSignals(True)
    self.ui.parameterNodeSelector.setCurrentNode(inputParameterNode)
    self.ui.parameterNodeSelector.blockSignals(wasBlocked)
    if inputParameterNode == self._parameterNode:
      return
    if self._parameterNode is not None:
      self.removeObserver(self._parameterNode, vtk.vtkCommand.ModifiedEvent, self.updateGUIFromParameterNode)
    if inputParameterNode is not None:
      self.addObserver(inputParameterNode, vtk.vtkCommand.ModifiedEvent, self.updateGUIFromParameterNode)
    self._parameterNode = inputParameterNode
    self.updateGUIFromParameterNode()

  def updateGUIFromParameterNode(self, caller=None, event=None):
    """
    This method is called whenever parameter node is changed.
    The module GUI is updated to show the current state of the parameter node.
    """

    # Disable all sections if no parameter node is selected
    self.ui.basicCollapsibleButton.enabled = self._parameterNode is not None
    if self._parameterNode is None:
      return

    # Update each widget from parameter node

    wasBlocked = self.ui.inputModelSelector1.blockSignals(True)
    self.ui.inputModelSelector1.setCurrentNode(self._parameterNode.GetNodeReference("InputModel1"))
    self.ui.inputModelSelector1.blockSignals(wasBlocked)

    wasBlocked = self.ui.inputModelSelector2.blockSignals(True)
    self.ui.inputModelSelector2.setCurrentNode(self._parameterNode.GetNodeReference("InputModel2"))
    self.ui.inputModelSelector2.blockSignals(wasBlocked)

    wasBlocked = self.ui.inputCurveSelector1.blockSignals(True)
    self.ui.inputCurveSelector1.setCurrentNode(self._parameterNode.GetNodeReference("InputCurve1"))
    self.ui.inputCurveSelector1.blockSignals(wasBlocked)

    wasBlocked = self.ui.inputCurveSelector2.blockSignals(True)
    self.ui.inputCurveSelector2.setCurrentNode(self._parameterNode.GetNodeReference("InputCurve2"))
    self.ui.inputCurveSelector2.blockSignals(wasBlocked)

    wasBlocked = self.ui.inputPlaneSelector1.blockSignals(True)
    self.ui.inputPlaneSelector1.setCurrentNode(self._parameterNode.GetNodeReference("InputPlane1"))
    self.ui.inputPlaneSelector1.blockSignals(wasBlocked)

    wasBlocked = self.ui.inputPlaneSelector2.blockSignals(True)
    self.ui.inputPlaneSelector2.setCurrentNode(self._parameterNode.GetNodeReference("InputPlane2"))
    self.ui.inputPlaneSelector2.blockSignals(wasBlocked)

    wasBlocked = self.ui.outputModelSelector.blockSignals(True)
    self.ui.outputModelSelector.setCurrentNode(self._parameterNode.GetNodeReference("OutputModel"))
    self.ui.outputModelSelector.blockSignals(wasBlocked)

    # Update buttons states and tooltips
    inputModelNode1 = self._parameterNode.GetNodeReference("InputModel1")
    inputModelNode2 = self._parameterNode.GetNodeReference("InputModel2")
    inputCurveNode1 = self._parameterNode.GetNodeReference("InputCurve1")
    inputCurveNode2 = self._parameterNode.GetNodeReference("InputCurve2")
    inputPlaneNode1 = self._parameterNode.GetNodeReference("InputPlane1")
    inputPlaneNode2 = self._parameterNode.GetNodeReference("InputPlane2")
    outputModelNode = self._parameterNode.GetNodeReference("OutputModel")
    if inputModelNode1 and inputModelNode2 and inputCurveNode1 and inputCurveNode2 and inputPlaneNode1 and inputPlaneNode2 and outputModelNode:
      self.ui.applyButton.toolTip = "Compute surface"
      self.ui.applyButton.enabled = True
    else:
      self.ui.applyButton.toolTip = "Select input and output model and curve nodes"
      self.ui.applyButton.enabled = False

  def updateParameterNodeFromGUI(self, caller=None, event=None):
    """
    This method is called when the user makes changes in the GUI.
    The changes are saved into the parameter node (so that they are preserved when the scene is saved and loaded).
    """
    if self._parameterNode is None:
      return

    inputModelNode1 = self.ui.inputModelSelector1.currentNodeID
    inputModelNode2 = self.ui.inputModelSelector2.currentNodeID
    inputCurveNode1 = self.ui.inputCurveSelector1.currentNodeID
    inputCurveNode2 = self.ui.inputCurveSelector2.currentNodeID
    inputPlaneNode1 = self.ui.inputPlaneSelector1.currentNodeID
    inputPlaneNode2 = self.ui.inputPlaneSelector2.currentNodeID
    outputModel = self.ui.outputModelSelector.currentNodeID
    
    self._parameterNode.SetNodeReferenceID("InputModel1", inputModelNode1)
    self._parameterNode.SetNodeReferenceID("InputModel2", inputModelNode2)
    self._parameterNode.SetNodeReferenceID("InputCurve1", inputCurveNode1)
    self._parameterNode.SetNodeReferenceID("InputCurve2", inputCurveNode2)
    self._parameterNode.SetNodeReferenceID("InputPlane1", inputPlaneNode1)
    self._parameterNode.SetNodeReferenceID("InputPlane2", inputPlaneNode2)
    self._parameterNode.SetNodeReferenceID("OutputModel", outputModel)

  def onApplyButton(self):
    """
    Run processing when user clicks "Apply" button.
    """
    inputModelNode1 = self._parameterNode.GetNodeReference("InputModel1")
    inputModelNode2 = self._parameterNode.GetNodeReference("InputModel2")
    inputCurveNode1 = self._parameterNode.GetNodeReference("InputCurve1")
    inputCurveNode2 = self._parameterNode.GetNodeReference("InputCurve2")
    inputPlaneNode1 = self._parameterNode.GetNodeReference("InputPlane1")
    inputPlaneNode2 = self._parameterNode.GetNodeReference("InputPlane2")
    outputModelNode = self._parameterNode.GetNodeReference("OutputModel")
    self.logic.run(inputModelNode1, inputModelNode2, inputCurveNode1, inputCurveNode2, inputPlaneNode1, inputPlaneNode2, outputModelNode)

#
# Mesher2Logic
#

class Mesher2Logic(ScriptedLoadableModuleLogic):
  """This class should implement all the actual
  computation done by your module.  The interface
  should be such that other python code can import
  this class and make use of the functionality without
  requiring an instance of the Widget.
  Uses ScriptedLoadableModuleLogic base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def run(self, inputModel1Node, inputModel2Node, inputCurveNode1, inputCurveNode2, inputPlaneNode1, inputPlaneNode2, outputModelNode):
    """
    Run the processing algorithm.
    Can be used without GUI widget.
    """
    if not inputModel1Node or not inputModel2Node or not inputCurveNode1 or not inputCurveNode2 or not inputPlaneNode1 or not inputPlaneNode2 or not outputModelNode:
      logging.error("Invalid inputs!")
      return

    logging.info('Processing started')

    inputModel1 = inputModel1Node.GetMesh()  # we should apply transform to World
    inputModel2 = inputModel2Node.GetMesh()  # we should apply transform to World
    inputCurve1 = inputCurveNode1.GetCurvePointsWorld()
    inputCurve2 = inputCurveNode2.GetCurvePointsWorld()

    normal1 = [0,0,0]
    normal2 = [0,0,0]
    origin1 = [0,0,0]
    inputPlane1.GetOriginWorld(origin1)
    origin2 = [0,0,0]
    inputPlane2.GetOriginWorld(origin2)
    
    vector1 = [0,0,0]
    vtk.vtkMath.Subtract(origin2, origin1)
    if (vtk.vtkMath.Dot(vector1, normal1) < 0):
      vtk.vtkMath.MultiplyScalar(normal1, -1)
    if (vtk.vtkMath.Dot(vector1, normal2) > 0):
      vtk.vtkMath.MultiplyScalar(normal2, -1)
    
    plane1 = vtk.vtkPlane()
    plane1.SetNormal(normal1)
    plane1.SetOrigin(origin1)
    
    cutter = vtk.vtkCutter()
    cutter.SetInputData(inputModel1)
    cutter.SetCutFunction(plane1)
    cutter.Update()
    cutModel1 = cutter.GetOutput()
    
    plane2 = vtk.vtkPlane()
    plane2.SetNormal(normal2)
    plane2.SetOrigin(origin2)
    
    cutter.SetInputData(inputModel2)
    cutter.SetCutFunction(plane2)
    cutter.Update()
    cutModel2 = cutter.GetOutput()
    
    inputCurve1 = self.combineCurves(inputCurve1, inputCurve2, inputPlaneNode1, inputPlaneNode2)

    #TODO
    line = vtk.vtkIdList()  
    for i in range(inputCurve1.GetNumberOfPoints()):
      line.InsertNextId(i)
      
    lines = vtk.vtkCellArray()
    lines.InsertNextCell(line)
    
    output = vtk.vtkPolyData()
    output.SetPoints(inputCurve1)
    output.SetLines(lines)
    outputModelNode.SetAndObservePolyData(output)
    return

    locator = vtk.vtkPointLocator()
    locator.SetDataSet(inputModel1)
    locator.BuildLocator()

    inputCurve2 = vtk.vtkPoints()    
    inputModel2Points = inputModel2.GetPoints()
    for i in range(inputCurve1.GetNumberOfPoints()):
      pointId = locator.FindClosestPoint(inputCurve1.GetPoint(i))
      inputCurve2.InsertNextPoint(inputModel2Points.GetPoint(pointId))

    inputPatch1 = self.getPatch(inputModel1, inputCurve1)
    normalFilter = vtk.vtkPolyDataNormals()
    normalFilter.SetInputData(inputPatch1)
    normalFilter.FlipNormalsOn()
    normalFilter.Update()
    inputPatch1 = normalFilter.GetOutput()
    inputPatch2 = self.getPatch(inputModel2, inputCurve2)

    append = vtk.vtkAppendPolyData()
    append.AddInputData(inputPatch1)
    append.AddInputData(inputPatch2)
    append.Update()
    output = self.stitchEdges(append.GetOutput(), inputCurve1, inputCurve2)

    normals = vtk.vtkPolyDataNormals()
    normals.SetInputData(output)
    normals.Update()

    outputModelNode.SetAndObservePolyData(normals.GetOutput())
    logging.info('Processing completed')

  def combineCurves(self, inputCurve1, inputCurve2, inputPlane1, inputPlane2):
    import numpy as np
    #curve1Segment = self.getLongestSegment(inputCurve1, inputPlane1, inputPlane2)
    #curve2Segment = self.getLongestSegment(inputCurve2, inputPlane1, inputPlane2)
    print(curve1Segment)
    print(curve2Segment)
    curve = vtk.vtkPoints()
    self.appendPoints(curve1Segment, curve, False)
    self.appendPoints(curve2Segment, curve, True)
    curve.InsertNextPoint(curve.GetPoint(0))
    return curve

  def appendPoints(self, inputPoints, outputPoints, invert):
    for i in range(inputPoints.GetNumberOfPoints()):
      if invert:
        point = inputPoints.GetPoint(inputPoints.GetNumberOfPoints()-1-i)
      else:
        point = inputPoints.GetPoint(i)
      outputPoints.InsertNextPoint(point)

  def getLongestSegment(self, curve, inputPlane1, inputPlane2):
    import numpy as np
  
    plane1Origin = [0,0,0]
    inputPlane1.GetOriginWorld(plane1Origin)
    plane1Normal = [0,0,0]
    inputPlane1.GetNormalWorld(plane1Normal)

    plane2Origin = [0,0,0]
    inputPlane2.GetOriginWorld(plane2Origin)
    plane2Normal = [0,0,0]
    inputPlane2.GetNormalWorld(plane2Normal)
  
    longestCurveSegment = []
    currentCurveSegment = []
    for i in range(curve.GetNumberOfPoints()-1):
      p0 = curve.GetPoint(i)
      p1 = curve.GetPoint(i+1)

      if len(currentCurveSegment) == 0:
        currentCurveSegment.append(p0)

      v0A = [0,0,0]
      vtk.vtkMath.Subtract(p0, plane1Origin, v0A)
      v1A = [0,0,0]
      vtk.vtkMath.Subtract(p1, plane1Origin, v1A)
      
      v0B = [0,0,0]
      vtk.vtkMath.Subtract(p0, plane2Origin, v0B)
      v1B = [0,0,0]
      vtk.vtkMath.Subtract(p1, plane2Origin, v1B)
      
      switchSegments = False
      if (np.sign(vtk.vtkMath.Dot(v0A, plane1Normal)) !=
           np.sign(vtk.vtkMath.Dot(v1A, plane1Normal))):
        d = vtk.vtkMath.Dot(v0A, plane1Normal)
        p1 = [0,0,0]
        vtk.vtkMath.Subtract(p0, [d*j for j in plane1Normal], p1)
        switchSegments = True
      elif (np.sign(vtk.vtkMath.Dot(v0B, plane2Normal)) !=
            np.sign(vtk.vtkMath.Dot(v1B, plane2Normal))):
        p1 = [0,0,0]
        vtk.vtkMath.Subtract(p0, [d*j for j in plane2Normal], p1)
        switchSegments = True

      currentCurveSegment.append(p1)
      if switchSegments:
        if len(currentCurveSegment) > len(longestCurveSegment):
          longestCurveSegment = currentCurveSegment
        currentCurveSegment = [p1] 
    
    currentCurveSegment.append(curve.GetPoint(curve.GetNumberOfPoints()-1))
    if len(currentCurveSegment) > len(longestCurveSegment):
      longestCurveSegment = currentCurveSegment    
    points = vtk.vtkPoints()
    for point in longestCurveSegment:
      points.InsertNextPoint(point)
    return points


  def stitchEdges(self, input, curve1, curve2):
    locator = vtk.vtkPointLocator()
    locator.SetDataSet(input)
    locator.BuildLocator()
    
    output = vtk.vtkPolyData()
    output.DeepCopy(input)
    polys = output.GetPolys()
    for i in range(curve1.GetNumberOfPoints()-1):
      p1A = locator.FindClosestPoint(curve1.GetPoint(i))
      p1B = locator.FindClosestPoint(curve1.GetPoint(i+1))
      p2A = locator.FindClosestPoint(curve2.GetPoint(i))
      p2B = locator.FindClosestPoint(curve2.GetPoint(i+1))
      
      triangle1 = vtk.vtkIdList()
      triangle1.InsertNextId(p1A)
      triangle1.InsertNextId(p1B)
      triangle1.InsertNextId(p2A)
      polys.InsertNextCell(triangle1)
      
      triangle2 = vtk.vtkIdList()
      triangle2.InsertNextId(p1B)
      triangle2.InsertNextId(p2B)
      triangle2.InsertNextId(p2A)
      polys.InsertNextCell(triangle2)
    return output

  def getPatch(self, inputModel, inputCurve):
    # Clip model with curve
    loop = vtk.vtkSelectPolyData()
    loop.SetLoop(inputCurve)
    loop.GenerateSelectionScalarsOn()
    loop.SetInputData(inputModel)
    loop.SetSelectionModeToLargestRegion()
    clip = vtk.vtkClipPolyData()
    clip.InsideOutOn()
    clip.SetInputConnection(loop.GetOutputPort())
    clip.GenerateClippedOutputOn()
    clip.Update()
    extractLargestPart = False
    clippedOutput = clip.GetOutput() if extractLargestPart else clip.GetClippedOutput()

    connectivity = vtk.vtkConnectivityFilter()
    connectivity.SetInputData(clippedOutput)
    connectivity.Update()
    clippedOutput2 = connectivity.GetOutput()
    
    # Remove unused points
    cleaner = vtk.vtkCleanPolyData()
    cleaner.SetInputData(clippedOutput2)
    cleaner.Update()
    return cleaner.GetOutput()


#
# Mesher2Test
#

class Mesher2Test(ScriptedLoadableModuleTest):
  """
  This is the test case for your scripted module.
  Uses ScriptedLoadableModuleTest base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    self.setUp()
    self.test_Mesher21()

  def test_Mesher21(self):
    """ Ideally you should have several levels of tests.  At the lowest level
    tests should exercise the functionality of the logic with different inputs
    (both valid and invalid).  At higher levels your tests should emulate the
    way the user would interact with your code and confirm that it still works
    the way you intended.
    One of the most important features of the tests is that it should alert other
    developers when their changes will have an impact on the behavior of your
    module.  For example, if a developer removes a feature that you depend on,
    your test should break so they know that the feature is needed.
    """

    self.delayDisplay("Starting the test")

    # Get/create input data

    import SampleData
    inputVolume = SampleData.downloadFromURL(
      nodeNames='MRHead',
      fileNames='MR-Head.nrrd',
      uris='https://github.com/Slicer/SlicerTestingData/releases/download/MD5/39b01631b7b38232a220007230624c8e',
      checksums='MD5:39b01631b7b38232a220007230624c8e')[0]
    self.delayDisplay('Finished with download and loading')

    inputScalarRange = inputVolume.GetImageData().GetScalarRange()
    self.assertEqual(inputScalarRange[0], 0)
    self.assertEqual(inputScalarRange[1], 279)

    outputVolume = slicer.mrmlScene.AddNewNodeByClass("vtkMRMLScalarVolumeNode")
    threshold = 50

    # Test the module logic

    logic = Mesher2Logic()

    # Test algorithm with non-inverted threshold
    logic.run(inputVolume, outputVolume, threshold, False)
    outputScalarRange = outputVolume.GetImageData().GetScalarRange()
    self.assertEqual(outputScalarRange[0], inputScalarRange[0])
    self.assertEqual(outputScalarRange[1], threshold)

    # Test algorithm with inverted threshold
    logic.run(inputVolume, outputVolume, threshold, True)
    outputScalarRange = outputVolume.GetImageData().GetScalarRange()
    self.assertEqual(outputScalarRange[0], inputScalarRange[0])
    self.assertEqual(outputScalarRange[1], inputScalarRange[1])

    self.delayDisplay('Test passed')
