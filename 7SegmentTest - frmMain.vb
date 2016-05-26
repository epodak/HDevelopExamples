'7SegmentTest.sln
'frmMain.vb

Option Explicit On      'require explicit declaration of variables, this is NOT Python !!
Option Strict On        'restrict implicit data type conversions to only widening conversions

Imports HalconDotNet

Public Class frmMain

    ' member variables ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Const XLD_SCALE_FACTOR As Double = 13.158

    Dim hwDxf As HWindow = Nothing
    Dim blnNeedToCallShapeModelMetric As Boolean = True
    Dim imgOriginal As New HImage()
    Dim xldModel As New HXLDCont()

    Private Sub btnOpenImage_Click(sender As Object, e As EventArgs) Handles btnOpenImage.Click
        hWindowControl.Dock = DockStyle.Fill        're-doc the HWindowControl (in case the function was already called once, its necessary to un-dock, see later in the function)
 
        Dim hWindow As HWindow                  'declare an HWindow object, we will connect this to the HWindowControl on the form in a minute . . .
        Dim hFramegrabber As HFramegrabber      'declare our frame grabber
         
        Dim intInitialWindowControlWidth As Integer = hWindowControl.Width      'get the initial HWindowControl width and height
        Dim intInitialWindowControlHeight As Integer = hWindowControl.Height
         
        hWindow = hWindowControl.HalconWindow         'associate HWindow object to HWindowControl object on the form
         
        Dim drChosenFile As DialogResult
 
        drChosenFile = openFileDialog.ShowDialog()                 'open file dialog
 
        If (drChosenFile <> DialogResult.OK Or openFileDialog.FileName = "") Then    'if user chose Cancel or filename is blank . . .
            lblChosenFile.Text = "file not chosen"              'show error message on label
            Return
        End If
 
        Try             'try to instiantiate the HFramegrabber object
            hFramegrabber = New HFramegrabber("File", 1, 1, 0, 0, 0, 0, "default", -1, "default", -1, "default", openFileDialog.FileName, "default", 1, -1)
        Catch ex As Exception
            lblChosenFile.Text = "unable to read image, error: " + ex.Message
            Return
        End Try
 
        lblChosenFile.Text = openFileDialog.FileName        'update the info label on the form showing which file was opened
 
        imgOriginal = hFramegrabber.GrabImage()     'grab the frame
         
        Dim imgResized As HImage = Nothing          'declare a resized image
 
                'get the resized image, as large as possible to fit the HWindowControl while still maintaining the image aspect ratio
        imgResized = resizeImageWhileMaintainingAspectRatio(imgOriginal, intInitialWindowControlWidth, intInitialWindowControlHeight)
         
        Dim htResizedImageWidth As HTuple = Nothing
        Dim htResizedImageHeight As HTuple = Nothing
 
        imgResized.GetImageSize(htResizedImageWidth, htResizedImageHeight)      'get the resized image width and height
 
        hWindowControl.Dock = DockStyle.None                    'undock the HWindowControl so we can change the size
 
        hWindowControl.Width = htResizedImageWidth              'change the HWindowControl size to match the resized image,
        hWindowControl.Height = htResizedImageHeight            'this is necessary b/c if we don't the HWindowControl will skew the image to fill itself entirely
 
        hWindow.SetPart(0, 0, htResizedImageHeight.I - 1, htResizedImageWidth.I - 1)        'set the HWindow object to show the the size of the resized image
         
        imgResized.DispObj(hWindow)                'display the image
        
    End Sub
    
    Private Sub btnOpenDxf_Click(sender As Object, e As EventArgs) Handles btnOpenDxf.Click

        If (Not hwDxf Is Nothing) Then
            hwDxf.CloseWindow()
        End If
        
        Dim drChosenFile As DialogResult
        drChosenFile = openFileDialog.ShowDialog()                 'open file dialog

        If (drChosenFile <> DialogResult.OK Or openFileDialog.FileName = "") Then    'if user chose Cancel or filename is blank . . .
            lblChosenFile.Text = "file not chosen"              'show error message on label
            Return
        End If
        
        Try
            xldModel.ReadContourXldDxf(openFileDialog.FileName, New HTuple(), New HTuple())
        Catch ex As Exception
            lblChosenFile.Text = "unable to read image, error: " + ex.Message
            Return
        End Try
        
        lblChosenFile.Text = openFileDialog.FileName

        Dim y1s As New HTuple()
        Dim x1s As New HTuple()
        Dim y2s As New HTuple()
        Dim x2s As New HTuple()

        xldModel.SmallestRectangle1Xld(y1s, x1s, y2s, x2s)

        Dim xs As HTuple = x1s.TupleConcat(x2s)
        Dim ys As HTuple = y1s.TupleConcat(y2s)

        Dim minX As HTuple = xs.TupleMin()
        Dim maxX As HTuple = xs.TupleMax()
        Dim minY As HTuple = ys.TupleMin()
        Dim maxY As HTuple = ys.TupleMax()

        Dim intXldModelWidth As Integer = CInt(maxX.TupleCeil().D) - CInt(minX.TupleFloor().D)
        Dim intXldModelHeight As Integer = CInt(maxY.TupleCeil().D) - CInt(minY.TupleFloor().D)

        Dim translateMatrix As New HHomMat2D()
        translateMatrix.HomMat2dIdentity()

        If (minX.D < 0) Then
            Dim absMinX As HTuple = minX.TupleAbs()
            translateMatrix = translateMatrix.HomMat2dTranslateLocal(absMinX, 0)
        End If

        If (minY.D < 0) Then
            Dim absMinY As HTuple = minY.TupleAbs()
            translateMatrix = translateMatrix.HomMat2dTranslateLocal(0, absMinY)
        End If

        xldModel = xldModel.AffineTransContourXld(translateMatrix)

        Dim intXldModelWidthScaled As Integer = CInt(intXldModelWidth * XLD_SCALE_FACTOR)
        Dim intXldModelHeightScaled As Integer = CInt(intXldModelHeight * XLD_SCALE_FACTOR)
        
        Dim scaleMatrix As New HHomMat2D()
        scaleMatrix.HomMat2dIdentity()
        scaleMatrix = scaleMatrix.HomMat2dScaleLocal(XLD_SCALE_FACTOR, XLD_SCALE_FACTOR)
        Dim xldModelScaled As HXLDCont = xldModel.AffineTransContourXld(scaleMatrix)
        
        hwDxf = New HWindow(150, 300, intXldModelWidthScaled, intXldModelHeightScaled, 0, "visible", "")

        hwDxf.SetPart(0, 0, intXldModelHeightScaled - 1, intXldModelWidthScaled - 1)
        
        xldModelScaled.DispObj(hwDxf)
        xldModel = xldModelScaled
    End Sub

    Function resizeImageWhileMaintainingAspectRatio(imgOrig As HImage, intWindowControlWidth As Integer, intWindowControlHeight As Integer) As HImage
        
        Dim imgResized As HImage = Nothing          'this will be the return values
 
        Dim htOriginalImageWidth As HTuple = Nothing        'declare tuples for image width and height
        Dim htOriginalImageHeight As HTuple = Nothing
         
        imgOrig.GetImageSize(htOriginalImageWidth, htOriginalImageHeight)     'get the image width and height
         
        Dim dblImageAspectRatio As Double = htOriginalImageWidth.D / htOriginalImageHeight.D
        
        Dim dblDeltaWidth As Double
        Dim dblDeltaHeight As Double
 
        dblDeltaWidth = CDbl(intWindowControlWidth - htOriginalImageWidth) / CDbl(htOriginalImageWidth)
        dblDeltaHeight = CDbl(intWindowControlHeight - htOriginalImageHeight) / CDbl(htOriginalImageHeight)
 
        Dim dblDelta As Double = Math.Min(dblDeltaWidth, dblDeltaHeight)
 
        Dim intNewImageWidth As Integer
        Dim intNewImageHeight As Integer
 
        intNewImageWidth = htOriginalImageWidth.I + CInt(htOriginalImageWidth.D * dblDelta)
        intNewImageHeight = htOriginalImageHeight.I + CInt(htOriginalImageHeight.D * dblDelta)
         
        imgResized = imgOrig.ZoomImageSize(intNewImageWidth, intNewImageHeight, "constant")
         
        Return imgResized
    End Function

    Private Sub btnFind_Click(sender As Object, e As EventArgs) Handles btnFind.Click
        If (imgOriginal.IsInitialized = False Or xldModel.IsInitialized = False) Then
            lblChosenFile.Text = "error, image or xld model not initialized"
            Return
        End If

        Dim hShapeModel As New HShapeModel()

        hShapeModel.CreateShapeModelXld(xldModel, "auto", -5.0 * (Math.PI * 180.0), 10.0 * (Math.PI * 180.0), "auto", "auto", "ignore_local_polarity", 5)

        Dim centerYs As New HTuple()
        Dim centerXs As New HTuple()
        Dim angles As New HTuple()
        Dim scores As New HTuple()

        hShapeModel.FindShapeModel(imgOriginal, -5.0 * (Math.PI * 180.0), 10.0 * (Math.PI * 180.0), 0.5, 1, 0.5, "least_squares", 0, 0.9, centerYs, centerXs, angles, scores)

        Dim homMat2D As New HHomMat2D()
        homMat2D.HomMat2dIdentity()
        homMat2D.VectorAngleToRigid(New HTuple(0), New HTuple(0), New HTuple(0), centerYs, centerXs, angles)
        Dim modelContours As HXLDCont = hShapeModel.GetShapeModelContours(1)
        xldModel = modelContours.AffineTransContourXld(homMat2D)

        Dim hWindow As HWindow = hWindowControl.HalconWindow
        hWindow.SetLineWidth(3)                 'set line width to 3, then draw the dot contours in red
        hWindow.SetColor("red")
        hWindow.DispXld(xldModel)

        Dim rows As New HTuple()
        Dim cols As New HTuple()
        Dim pointOrders As New HTuple()

        Dim area As HTuple = xldModel.AreaCenterXld(rows, cols, pointOrders)
        
        hWindow.DispXld(xldModel)
        
    End Sub

End Class






