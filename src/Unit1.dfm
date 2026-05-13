object Form1: TForm1
  Left = 299
  Top = 52
  Width = 1079
  Height = 775
  Align = alLeft
  Caption = 'PL/0 Compiler'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 20
  object Label1: TLabel
    Left = 832
    Top = 32
    Width = 160
    Height = 40
    Caption = #28304#31243#24207#21517
    Font.Charset = GB2312_CHARSET
    Font.Color = clWindowText
    Font.Height = -40
    Font.Name = #40657#20307
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 825
    Top = 500
    Width = 180
    Height = 30
    Caption = '   3123004857   '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 865
    Top = 610
    Width = 100
    Height = 33
    Caption = '  '#26366#20255#32988'  '
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -32
    Font.Name = #20223#23435
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label5: TLabel
    Left = 24
    Top = 0
    Width = 140
    Height = 20
    Caption = ' '#24320#22987#35843#35797#26102#38388#65306'      '
  end
  object Label6: TLabel
    Left = 248
    Top = 0
    Width = 59
    Height = 20
    Caption = '2026-05'
  end
  object Label7: TLabel
    Left = 16
    Top = 40
    Width = 164
    Height = 20
    Caption = '   '#32467#26463#35843#35797#26102#38388#65306'          '
  end
  object Label8: TLabel
    Left = 248
    Top = 40
    Width = 80
    Height = 20
    Caption = '2026-07'
  end
  object Label9: TLabel
    Left = 800
    Top = 555
    Width = 230
    Height = 33
    Caption = '  '#20154#24037#26234#33021#21019#26032#29677'  '
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -32
    Font.Name = #20223#23435
    Font.Style = [fsBold]
    ParentFont = False
  end
  object ButtonRun: TButton
    Left = 844
    Top = 395
    Width = 139
    Height = 54
    Caption = 'RUN'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -40
    Font.Name = 'Times New Roman MT Extra Bold'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnClick = ButtonRunClick
  end
  object Memo1: TMemo
    Left = 8
    Top = 64
    Width = 769
    Height = 673
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -40
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ImeName = #32043#20809#25340#38899#36755#20837#27861
    Lines.Strings = (
      '***** PL/0 Compiler Demo *****')
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 1
  end
  object ComboCase: TComboBox
    Left = 815
    Top = 82
    Width = 200
    Height = 28
    Style = csDropDown
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 20
    ParentFont = False
    TabOrder = 2
    Text = 'E01'
  end
  object ButtonRefresh: TButton
    Left = 815
    Top = 115
    Width = 100
    Height = 25
    Caption = #21047#26032#21015#34920
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    OnClick = ButtonRefreshClick
  end
  object ButtonClear: TButton
    Left = 920
    Top = 115
    Width = 95
    Height = 25
    Caption = #28165#31354#36755#20986
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    OnClick = ButtonClearClick
  end
  object ListSwitch: TRadioGroup
    Left = 824
    Top = 168
    Width = 177
    Height = 163
    Caption = #30446#26631#20195#30721
    Font.Charset = GB2312_CHARSET
    Font.Color = clWindowText
    Font.Height = -40
    Font.Name = #40657#20307
    Font.Style = []
    ItemIndex = 0
    Items.Strings = (
      #26174#31034
      #19981#26174#31034)
    ParentFont = False
    TabOrder = 3
  end
end
