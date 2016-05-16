<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.hWindowControl = New HalconDotNet.HWindowControl()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.btnOpenFile = New System.Windows.Forms.Button()
        Me.lblChosenFile = New System.Windows.Forms.Label()
        Me.openFileDialog = New System.Windows.Forms.OpenFileDialog()
        Me.TableLayoutPanel1.SuspendLayout
        Me.SuspendLayout
        '
        'hWindowControl
        '
        Me.hWindowControl.BackColor = System.Drawing.Color.Black
        Me.hWindowControl.BorderColor = System.Drawing.Color.Black
        Me.TableLayoutPanel1.SetColumnSpan(Me.hWindowControl, 2)
        Me.hWindowControl.Dock = System.Windows.Forms.DockStyle.Fill
        Me.hWindowControl.ImagePart = New System.Drawing.Rectangle(0, 0, 640, 480)
        Me.hWindowControl.Location = New System.Drawing.Point(3, 44)
        Me.hWindowControl.Name = "hWindowControl"
        Me.hWindowControl.Size = New System.Drawing.Size(1265, 831)
        Me.hWindowControl.TabIndex = 0
        Me.hWindowControl.WindowSize = New System.Drawing.Size(1265, 831)
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 2
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle())
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100!))
        Me.TableLayoutPanel1.Controls.Add(Me.hWindowControl, 0, 1)
        Me.TableLayoutPanel1.Controls.Add(Me.btnOpenFile, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.lblChosenFile, 1, 0)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 2
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle())
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(1271, 878)
        Me.TableLayoutPanel1.TabIndex = 1
        '
        'btnOpenFile
        '
        Me.btnOpenFile.Anchor = CType((System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right),System.Windows.Forms.AnchorStyles)
        Me.btnOpenFile.AutoSize = true
        Me.btnOpenFile.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.btnOpenFile.Font = New System.Drawing.Font("Microsoft Sans Serif", 12!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0,Byte))
        Me.btnOpenFile.Location = New System.Drawing.Point(3, 3)
        Me.btnOpenFile.Name = "btnOpenFile"
        Me.btnOpenFile.Size = New System.Drawing.Size(107, 35)
        Me.btnOpenFile.TabIndex = 1
        Me.btnOpenFile.Text = "Open File"
        Me.btnOpenFile.UseVisualStyleBackColor = true
        '
        'lblChosenFile
        '
        Me.lblChosenFile.Anchor = CType((System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right),System.Windows.Forms.AnchorStyles)
        Me.lblChosenFile.AutoSize = true
        Me.lblChosenFile.Font = New System.Drawing.Font("Microsoft Sans Serif", 12!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0,Byte))
        Me.lblChosenFile.Location = New System.Drawing.Point(116, 8)
        Me.lblChosenFile.Name = "lblChosenFile"
        Me.lblChosenFile.Size = New System.Drawing.Size(1152, 25)
        Me.lblChosenFile.TabIndex = 2
        Me.lblChosenFile.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'openFileDialog
        '
        Me.openFileDialog.FileName = "OpenFileDialog1"
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(8!, 16!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1271, 878)
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Name = "frmMain"
        Me.Text = "Form1"
        Me.TableLayoutPanel1.ResumeLayout(false)
        Me.TableLayoutPanel1.PerformLayout
        Me.ResumeLayout(false)

End Sub

    Friend WithEvents hWindowControl As HalconDotNet.HWindowControl
    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents btnOpenFile As Button
    Friend WithEvents lblChosenFile As Label
    Friend WithEvents openFileDialog As OpenFileDialog
End Class
