'HalconCountDiceDotsVB.sln
'frmMain.vb

Option Explicit On      'require explicit declaration of variables, this is NOT Python !!
Option Strict On        'restrict implicit data type conversions to only widening conversions

Imports HalconDotNet

Public Class frmMain

    Private Sub btnOpenFile_Click(sender As Object, e As EventArgs) Handles btnOpenFile.Click

        hWindowControl.Dock = DockStyle.Fill        're-doc the HWindowControl (in case the function was already called once, its necessary to un-dock, see later in the function)

        Dim hWindow As HWindow                  'declare an HWindow object, we will connect this to the HWindowControl on the form in a minute . . .
        Dim hFramegrabber As HFramegrabber      'declare our frame grabber
        Dim imgOriginal As HImage = Nothing     'declare our original image

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
        
        Dim regThresh As HRegion = Nothing                  'declare the threshold region
        
        regThresh = imgResized.Threshold(80.0, 255.0)      'get the threshold region

        Dim dieRegions As HRegion = Nothing             'declare the die regions
        
        dieRegions = regThresh.Connection()             'breakout the threshold region into an array of separate regions, one for each individual die
        
        Dim htNumDotsOnEachDie As HTuple = Nothing          'declare tuple for number of dots on each die

        dieRegions.ConnectAndHoles(htNumDotsOnEachDie)      'find the number of holes, which is the same as the number of dots, in each die region
        
        Dim dieContours As New HXLDCont                        'declare contours
        Dim dieAndDotContours As New HXLDCont
        Dim dotContours As New HXLDCont

        dieContours = dieRegions.GenContourRegionXld("border")                      'first get just the die contours

        dieAndDotContours = dieRegions.GenContourRegionXld("border_holes")          'next get die and dot contours

                'finally subtract the die contours from the die and dot contours to get just the dot contours
        dotContours = dieAndDotContours.SymmDifferenceClosedContoursXld(dieContours)

        Dim strDarkGreen As String = "#00C800"              'declare a string constant for the dark green color

        hWindow.SetLineWidth(3)                 'set line width to 3, then draw the die contours in dark green
        hWindow.SetColor(strDarkGreen)
        hWindow.DispXld(dieContours)

        hWindow.SetLineWidth(3)                 'set line width to 3, then draw the dot contours in red
        hWindow.SetColor("red")
        hWindow.DispXld(dotContours)
        
        Dim htDieCenterYs As HTuple = Nothing
        Dim htDieCenterXs As HTuple = Nothing
        
        dieRegions.AreaCenter(htDieCenterYs, htDieCenterXs)         'for each die get the center X and Y values

        Dim htDieRectY1s As HTuple = Nothing
        Dim htDieRectX1s As HTuple = Nothing
        Dim htDieRectY2s As HTuple = Nothing
        Dim htDieRectX2s As HTuple = Nothing

                'get the bounding rect for each die, this function gives the top left point X and Y, and the bottom right point X and Y
        HOperatorSet.SmallestRectangle1(dieRegions, htDieRectY1s, htDieRectX1s, htDieRectY2s, htDieRectX2s)
        
        Dim intFontSize As Integer = CInt(htResizedImageHeight.D * 0.06)       'declare variables to pass into set Halcon font function
        Dim strFont As String = "mono"
        Dim blnBold As Boolean = True
        Dim blnSlant As Boolean = False

        setHalconFont(hWindow, intFontSize, strFont, blnBold, blnSlant)
        
        Dim strTextToWrite As String            'declare variables to pass into write text on Halcon window function
        Dim strCoordSystem As String
        Dim intTopLeftYPos As Integer
        Dim intTopLeftXPos As Integer
        Dim strColor As String
        Dim strBox As String
        
        For i As Integer = 0 To htNumDotsOnEachDie.Length - 1                   'for each die

            strTextToWrite = htNumDotsOnEachDie(i).I.ToString()         'set up variables to write on window
            strCoordSystem = "image"
            intTopLeftYPos = CInt(htDieCenterYs(i).D)
            intTopLeftXPos = CInt(htDieCenterXs(i).D + ((htDieRectX2s(i).D - htDieRectX1s(i)).D * 0.65))
            strColor = "red"
            strBox = "false"
                    'write the dot count next to each die in red
            writeTextOnHalconWindow(hWindow, strTextToWrite, strCoordSystem, intTopLeftYPos, intTopLeftXPos, strColor, strBox)
        Next
        
        Dim htTotalDotCount As HTuple = Nothing

        HOperatorSet.TupleSum(htNumDotsOnEachDie, htTotalDotCount)          'get the total dot count
        
        strTextToWrite = "Sum: " + htTotalDotCount.ToString()           'set up variables to write on window
        strCoordSystem = "image"
        intTopLeftYPos = CInt(htResizedImageHeight.D * 0.015)
        intTopLeftXPos = CInt(htResizedImageWidth.D * 0.015)
        strColor = "#00C800"
        strBox = "false"
                'write the total dot count at the top left of the image in green
        writeTextOnHalconWindow(hWindow, strTextToWrite, strCoordSystem, intTopLeftYPos, intTopLeftXPos, strColor, strBox)

    End Sub

    Function resizeImageWhileMaintainingAspectRatio(imgOriginal As HImage, intWindowControlWidth As Integer, intWindowControlHeight As Integer) As HImage

        Dim imgResized As HImage = Nothing          'this will be the return values

        Dim htOriginalImageWidth As HTuple = Nothing        'declare tuples for image width and height
        Dim htOriginalImageHeight As HTuple = Nothing
        
        imgOriginal.GetImageSize(htOriginalImageWidth, htOriginalImageHeight)     'get the image width and height
        
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
        
        imgResized = imgOriginal.ZoomImageSize(intNewImageWidth, intNewImageHeight, "constant")
        
        Return imgResized
    End Function
    
    Sub writeTextOnHalconWindow(hWindow As HWindow, strTextToWrite As String, strCoordSystem As String, intTopLeftYPos As Integer, intTopLeftXPos As Integer, strColor As String, strBox As String)

        If (strCoordSystem = "window") Then
            'add more here later
        ElseIf (strCoordSystem = "image") Then
            'add more here later
        Else
            'should never get here
        End If
        
        hWindow.SetTposition(intTopLeftYPos, intTopLeftXPos)

        hWindow.SetColor(strColor)

        'add strBox here later

        hWindow.WriteString(strTextToWrite)
    End Sub
    
    Sub setHalconFont(hWindow As HWindow, intFontSize As Integer, strFont As String, blnBold As Boolean, blnSlant As Boolean)

        Dim strFinalFontString As String = ""       'this will be the eventual fully build up string to pass into SetFont
        
                                                'the font passed into this function is the Halcon name for a font, ex. 'mono'
        Dim strConvertedFont As String = ""     'this variable holds the actual operating system name for the corresponding font

        If (strFont = "mono") Then
            strConvertedFont = "Consolas"
        Else
            'put more conditionals for font options other than mono here later
        End If
        
        strFinalFontString = "-" + strConvertedFont + "-" + intFontSize.ToString() + "-*-"        'add the choice for font and size

        If (blnSlant = True) Then                           'add the choice for slant (italicized)
            strFinalFontString = strFinalFontString + "1-*-*-"
        ElseIf (blnSlant = False) Then
            strFinalFontString = strFinalFontString + "0-*-*-"
        Else
            'should never get here
        End If

        If (blnBold = True) Then
            strFinalFontString = strFinalFontString + "1-"
        ElseIf (blnBold = False) Then
            strFinalFontString = strFinalFontString + "0-"
        Else
            'should never get here
        End If
        
        hWindow.SetFont(strFinalFontString)

    End Sub
    
End Class
