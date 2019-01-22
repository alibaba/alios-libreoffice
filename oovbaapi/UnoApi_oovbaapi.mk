# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

$(eval $(call gb_UnoApi_UnoApi,oovbaapi))

$(eval $(call gb_UnoApi_use_api,oovbaapi,\
    udkapi \
    offapi \
))

$(eval $(call gb_UnoApi_add_idlfiles_nohdl,oovbaapi,ooo/vba/excel,\
	Globals \
	Hyperlink \
	Range \
	TextFrame \
	Window \
	Workbook \
	Worksheet \
))

$(eval $(call gb_UnoApi_add_idlfiles_noheader,oovbaapi,ooo/vba,\
	ControlProvider \
))
$(eval $(call gb_UnoApi_add_idlfiles_noheader,oovbaapi,ooo/vba/excel,\
	Button \
	Buttons \
))

$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba,\
    FormShowConstants \
    VbAppWinStyle \
    VbCalendar \
    VbCallType \
    VbCompareMethod \
    VbDateTimeFormat \
    VbDayOfWeek \
    VbFileAttribute \
    VbFirstWeekOfYear \
    VbIMEStatus \
    VbMsgBoxResult \
    VbMsgBoxStyle \
    VbQueryClose \
    VbStrConv \
    SystemColorConstants \
    VbTriState \
    VbVarType \
	XApplicationBase \
	XAssistant \
	XCollection \
	XCollectionBase \
	XCommandBar \
	XCommandBarButton \
	XCommandBarControl \
	XCommandBarControls \
	XCommandBarPopup \
	XCommandBars \
	XControlProvider \
	XDialogBase \
	XDialogsBase \
	XDocumentBase \
	XDocumentProperties \
	XDocumentProperty \
	XDocumentsBase \
	XErrObject \
	XExecutableDialog \
	XFontBase \
	XGlobalsBase \
	XHelperInterface \
	XPageSetupBase \
	XPropValue \
	XVBAToOOEventDescGen \
	XWindowBase \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/access,\
    AcCloseSave \
    AcColorIndex \
    AcCommand \
    AcControlType \
    AcCurrentView \
    AcDataAccessPageView \
    AcDataObjectType \
    AcDataTransferType \
    AcDefView \
    AcExportXMLEncoding \
    AcExportXMLObjectType \
    AcExportXMLOtherFlags \
    AcExportXMLSchemaFormat \
    AcFileFormat \
    AcFilterType \
    AcFindField \
    AcFindMatch \
    AcFormatConditionOperator \
    AcFormatConditionType \
    AcFormOpenDataMode \
    AcFormView \
    AcHyperlinkPart \
    AcImeMode \
    AcImeSentenceMode \
    AcImportXMLOption \
    AcModuleType \
    AcObjectType \
    AcOpenDataMode \
    AcOutputObjectType \
    AcPrintColor \
    AcPrintDuplex \
    AcPrintItemLayout \
    AcPrintObjQuality \
    AcPrintOrientation \
    AcPrintPaperBin \
    AcPrintPaperSize \
    AcPrintQuality \
    AcPrintRange \
    AcProjectType \
    AcQuitOption \
    AcRecord \
    AcSearchDirection \
    AcSection \
    AcSendObjectType \
    AcShowToolbar \
    AcSpreadSheetType \
    AcSysCmdAction \
    AcTextTransferType \
    AcTransformXMLScriptOption \
    AcView \
    AcWindowMode \
    ProcKind \
    RefKind \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/adodb,\
    ADCPROP_ASYNCTHREADPRIORITY_ENUM \
    ADCPROP_AUTORECALC_ENUM \
    ADCPROP_UPDATECRITERIA_ENUM \
    AffectEnum \
    BookmarkEnum \
    CEResyncEnum \
    CommandTypeEnum \
    CompareEnum \
    ConnectModeEnum \
    ConnectOptionEnum \
    ConnectPromptEnum \
    CursorLocationEnum \
    CursorOptionEnum \
    CursorTypeEnum \
    DataTypeEnum \
    EditModeEnum \
    ErrorValueEnum \
    EventReasonEnum \
    EventStatusEnum \
    ExecuteOptionEnum \
    FieldAttributeEnum \
    FilterGroupEnum \
    GetRowsOptionEnum \
    IsolationLevelEnum \
    LockTypeEnum \
    MarshalOptionsEnum \
    ObjectStateEnum \
    ParameterAttributesEnum \
    ParameterDirectionEnum \
    PersistFormatEnum \
    PositionEnum \
    PropertyAttributesEnum \
    RecordStatusEnum \
    ResyncEnum \
    SchemaEnum \
    SearchDirectionEnum \
    SeekEnum \
    StringFormatEnum \
    XactAttributeEnum \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/dao,\
    CollatingOrderEnum \
    CommitTransOptionsEnum \
    CursorDriverEnum \
    DatabaseTypeEnum \
    DataTypeEnum \
    DriverPromptEnum \
    EditModeEnum \
    FieldAttributeEnum \
    IdleEnum \
    LockTypeEnum \
    ParameterDirectionEnum \
    PermissionEnum \
    QueryDefStateEnum \
    QueryDefTypeEnum \
    RecordsetOptionEnum \
    RecordsetTypeEnum \
    RecordStatusEnum \
    RelationAttributeEnum \
    ReplicaTypeEnum \
    SetOptionEnum \
    SynchronizeTypeEnum \
    TableDefAttributeEnum \
    UpdateCriteriaEnum \
    UpdateTypeEnum \
    WorkspaceTypeEnum \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/excel,\
    Constants \
	XApplication \
	XAxes \
	XAxis \
	XAxisTitle \
	XBorder \
	XBorders \
	XButton \
	XCharacters \
	XChart \
	XChartObject \
	XChartObjects \
	XCharts \
	XChartTitle \
	XComment \
	XComments \
	XControlObject \
	XDataLabel \
	XDataLabels \
	XDialog \
	XDialogs \
	XDrawings \
	XFileDialog \
	XFileDialogSelectedItems \
	XFont \
	XFormat \
	XFormatCondition \
	XFormatConditions \
	XGlobals \
	XGraphicObjects \
	XHPageBreak \
	XHPageBreaks \
	XHyperlink \
	XHyperlinks \
	XInterior \
	XLineObjects \
    XlApplicationInternational \
    XlApplyNamesOrder \
    XlArabicModes \
    XlArrangeStyle \
    XlArrowHeadLength \
    XlArrowHeadStyle \
    XlArrowHeadWidth \
    XlAutoFillType \
    XlAutoFilterOperator \
    XlAxisCrosses \
    XlAxisGroup \
    XlAxisType \
    XlBackground \
    XlBarShape \
    XlBordersIndex \
    XlBorderWeight \
	XlBuildInDialog \
    XlBuiltInDialog \
    XlCalculatedMemberType \
    XlCalculation \
    XlCalculationInterruptKey \
    XlCalculationState \
    XlCategoryType \
    XlCellInsertionMode \
    XlCellType \
    XlChartGallery \
    XlChartItem \
    XlChartLocation \
    XlChartPicturePlacement \
    XlChartPictureType \
    XlChartSplitType \
    XlChartType \
    XlClipboardFormat \
    XlCmdType \
    XlColorIndex \
    XlColumnDataType \
    XlCommandUnderlines \
    XlCommentDisplayMode \
    XlConsolidationFunction \
    XlCopyPictureFormat \
    XlCorruptLoad \
    XlCreator \
    XlCubeFieldType \
    XlCutCopyMode \
    XlCVError \
    XlDataLabelPosition \
    XlDataLabelSeparator \
    XlDataLabelsType \
    XlDataSeriesDate \
    XlDataSeriesType \
    XlDeleteShiftDirection \
    XlDirection \
    XlDisplayBlanksAs \
    XlDisplayDrawingObjects \
    XlDisplayUnit \
    XlDVAlertStyle \
    XlDVType \
    XlEditionFormat \
    XlEditionOptionsOption \
    XlEditionType \
    XlEnableCancelKey \
    XlEnableSelection \
    XlEndStyleCap \
    XlErrorBarDirection \
    XlErrorBarInclude \
    XlErrorBarType \
    XlErrorChecks \
    XlFileAccess \
    XlFileFormat \
    XlFillWith \
    XlFilterAction \
    XlFindLookIn \
    XlFormatConditionOperator \
    XlFormatConditionType \
    XlFormControl \
    XlFormulaLabel \
    XlHAlign \
    XlHebrewModes \
    XlHighlightChangesTime \
    XlHtmlType \
    XlIMEMode \
    XlImportDataAs \
    XlInsertFormatOrigin \
    XlInsertShiftDirection \
    XlLayoutFormType \
    XlLegendPosition \
    XlLineStyle \
    XlLink \
    XlLinkInfo \
    XlLinkInfoType \
    XlLinkStatus \
    XlLinkType \
    XlListConflict \
    XlListDataType \
    XlListObjectSourceType \
    XlLocationInTable \
    XlLookAt \
    XlMailSystem \
    XlMarkerStyle \
    XlMouseButton \
    XlMousePointer \
    XlMSApplication \
    XlObjectSize \
    XlOLEType \
    XlOLEVerb \
    XlOrder \
    XlOrientation \
    XlPageBreak \
    XlPageBreakExtent \
    XlPageOrientation \
    XlPaperSize \
    XlParameterDataType \
    XlParameterType \
    XlPasteSpecialOperation \
    XlPasteType \
    XlPattern \
    XlPhoneticAlignment \
    XlPhoneticCharacterType \
    XlPictureAppearance \
    XlPictureConvertorType \
    XlPivotCellType \
    XlPivotFieldCalculation \
    XlPivotFieldDataType \
    XlPivotFieldOrientation \
    XlPivotFormatType \
    XlPivotTableMissingItems \
    XlPivotTableSourceType \
    XlPivotTableVersionList \
    XlPlacement \
    XlPlatform \
    XlPrintErrors \
    XlPrintLocation \
    XlPriority \
    XlPTSelectionMode \
    XlQueryType \
    XlRangeAutoFormat \
    XlRangeValueDataType \
    XlReferenceStyle \
    XlReferenceType \
    XlRobustConnect \
    XlRoutingSlipDelivery \
    XlRoutingSlipStatus \
    XlRowCol \
    XlRunAutoMacro \
    XlSaveAction \
    XlSaveAsAccessMode \
    XlSaveConflictResolution \
    XlScaleType \
    XlSearchDirection \
    XlSearchOrder \
    XlSearchWithin \
    XlSheetType \
    XlSheetVisibility \
    XlSizeRepresents \
    XlSmartTagControlType \
    XlSmartTagDisplayMode \
    XlSortDataOption \
    XlSortMethod \
    XlSortMethodOld \
    XlSortOrder \
    XlSortOrientation \
    XlSortType \
    XlSourceType \
    XlSpeakDirection \
    XlSpecialCellsValue \
    XlSubscribeToFormat \
    XlSubtototalLocationType \
    XlSummaryColumn \
    XlSummaryReportType \
    XlSummaryRow \
    XlTabPosition \
    XlTextParsingType \
    XlTextQualifier \
    XlTextVisualLayoutType \
    XlTickLabelOrientation \
    XlTickLabelPosition \
    XlTickMark \
    XlTimeUnit \
    XlToolbarProtection \
    XlTotalsCalculation \
    XlTrendlineType \
    XlUnderlineStyle \
    XlUpdateLinks \
    XlVAlign \
    XlWBATemplate \
    XlWebFormatting \
    XlWebSelectionType \
    XlWindowState \
    XlWindowType \
    XlWindowView \
    XlXLMMacroType \
    XlXmlExportResult \
    XlXmlImportResult \
    XlXmlLoadOption \
    XlYesNoGuess \
	XMenu \
	XMenuBar \
	XMenuBars \
	XMenuItem \
	XMenuItems \
	XMenus \
	XName \
	XNames \
	XOLEObject \
	XOLEObjects \
	XOutline \
	XPageBreak \
	XPageSetup \
	XPane \
	XPivotCache \
	XPivotTable \
	XPivotTables \
	XRange \
	XSeries \
	XSeriesCollection \
	XSheetObject \
	XStyle \
	XStyles \
	XTextFrame \
	XTitle \
	XValidation \
	XVPageBreak \
	XVPageBreaks \
	XWindow \
	XWindows \
	XWorkbook \
	XWorkbooks \
	XWorksheet \
	XWorksheetFunction \
	XWorksheets \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/msforms,\
    fmAction \
    fmAlignment \
    fmBackStyle \
    fmBorders \
    fmBorderStyle \
    fmButtonEffect \
    fmButtonStyle \
    fmCycle \
    fmDisplayStyle \
    fmDragBehavior \
    fmDragState \
    fmDropButtonStyle \
    fmDropEffect \
    fmEnterFieldBehavior \
    fmIMEMode \
    fmLayoutEffect \
    fmListStyle \
    fmMatchEntry \
    fmMode \
    fmMousePointer \
    fmMultiSelect \
    fmOrientation \
    fmPicPosition \
    fmPictureAlignment \
    fmPicturePosition \
    fmPictureSizeMode \
    fmScrollAction \
    fmScrollBars \
    fmShowDropButtonWhen \
    fmShowListWhen \
    fmSnapPoint \
    fmSpecialEffect \
    fmStyle \
    fmTabOrientation \
    fmTabStyle \
    fmTextAlign \
    fmTransitionEffect \
    fmVerticalScrollBarSide \
    fmZOrder \
	XButton \
	XCheckBox \
	XColorFormat \
	XComboBox \
	XCommandButton \
	XControl \
	XControls \
	XFillFormat \
	XFrame \
	XGroupBox \
	XImage \
	XLabel \
	XLine \
	XLineFormat \
	XListBox \
	XMultiPage \
	XNewFont \
	XOval \
	XPages \
	XPictureFormat \
	XProgressBar \
	XRadioButton \
	XReturnBoolean \
	XReturnEffect \
	XReturnInteger \
	XReturnSingle \
	XScrollBar \
	XShape \
	XShapeRange \
	XShapes \
	XSpinButton \
	XTextBox \
	XTextBoxShape \
	XTextFrame \
	XToggleButton \
	XUserForm \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/office,\
    DocProperties \
    MailFormat \
    MsoAlertButtonType \
    MsoAlertCancelType \
    MsoAlertDefaultType \
    MsoAlertIconType \
    MsoAlignCmd \
    MsoAnimationType \
    MsoAppLanguageID \
    MsoArrowheadLength \
    MsoArrowheadStyle \
    MsoArrowheadWidth \
    MsoAutomationSecurity \
    MsoAutoShapeType \
    MsoBalloonButtonType \
    MsoBalloonErrorType \
    MsoBalloonType \
    MsoBarPosition \
    MsoBarProtection \
    MsoBarRow \
    MsoBarType \
    MsoBlackWhiteMode \
    MsoButtonSetType \
    MsoButtonState \
    MsoButtonStyle \
    MsoButtonStyleHidden \
    MsoCalloutAngleType \
    MsoCalloutDropType \
    MsoCalloutType \
    MsoCharacterSet \
    MsoColorType \
    MsoComboStyle \
    MsoCommandBarButtonHyperlinkType \
    MsoCondition \
    MsoConnector \
    MsoConnectorType \
    MsoControlOLEUsage \
    MsoControlType \
    MsoDiagramNodeType \
    MsoDiagramType \
    MsoDistributeCmd \
    MsoDocProperties \
    MsoEditingType \
    MsoEncoding \
    MsoExtraInfoMethod \
    MsoExtrusionColorType \
    MsoFarEastLineBreakLanguageID \
    MsoFeatureInstall \
    MsoFileDialogType \
    MsoFileDialogView \
    MsoFileFindListBy \
    MsoFileFindOptions \
    MsoFileFindSortBy \
    MsoFileFindView \
    MsoFileNewAction \
    MsoFileNewSection \
    MsoFileType \
    MsoFillType \
    MsoFilterComparison \
    MsoFilterConjunction \
    MsoFlipCmd \
    MsoGradientColorType \
    MsoGradientStyle \
    MsoHorizontalAnchor \
    MsoHTMLProjectOpen \
    MsoHTMLProjectState \
    MsoHyperlinkType \
    MsoIconType \
    MsoLanguageID \
    MsoLanguageIDHidden \
    MsoLastModified \
    MsoLineDashStyle \
    MsoLineStyle \
    MsoMenuAnimation \
    MsoMixedType \
    MsoModeType \
    MsoMoveRow \
    MsoOLEMenuGroup \
    MsoOrgChartLayoutType \
    MsoOrgChartOrientation \
    MsoOrientation \
    MsoPatternType \
    MsoPermission \
    MsoPictureColorType \
    MsoPresetExtrusionDirection \
    MsoPresetGradientType \
    MsoPresetLightingDirection \
    MsoPresetLightingSoftness \
    MsoPresetMaterial \
    MsoPresetTextEffect \
    MsoPresetTextEffectShape \
    MsoPresetTexture \
    MsoPresetThreeDFormat \
    MsoRelativeNodePosition \
    MsoScaleFrom \
    MsoScreenSize \
    MsoScriptLanguage \
    MsoScriptLocation \
    MsoSearchIn \
    MsoSegmentType \
    MsoShadowType \
    MsoShapeType \
    MsoSharedWorkspaceTaskPriority \
    MsoSharedWorkspaceTaskStatus \
    MsoSortBy \
    MsoSortOrder \
    MsoSyncAvailableType \
    MsoSyncCompareType \
    MsoSyncConflictResolutionType \
    MsoSyncErrorType \
    MsoSyncEventType \
    MsoSyncStatusType \
    MsoSyncVersionType \
    MsoTargetBrowser \
    MsoTextEffectAlignment \
    MsoTextOrientation \
    MsoTextureType \
    MsoTriState \
    MsoVerticalAnchor \
    MsoWizardActType \
    MsoWizardMsgType \
    MsoZOrderCmd \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/powerpoint,\
    MsoAnimAccumulate \
    MsoAnimAdditive \
    MsoAnimAfterEffect \
    MsoAnimateByLevel \
    MsoAnimCommandType \
    MsoAnimDirection \
    MsoAnimEffect \
    MsoAnimEffectAfter \
    MsoAnimEffectRestart \
    MsoAnimFilterEffectSubtype \
    MsoAnimFilterEffectType \
    MsoAnimProperty \
    MsoAnimTextUnitEffect \
    MsoAnimTriggerType \
    MsoAnimType \
    PpActionType \
    PpAdvanceMode \
    PpAfterEffect \
    PpAlertLevel \
    PpArrangeStyle \
    PpAutoSize \
    PpBaselineAlignment \
    PpBorderType \
    PpBulletType \
    PpChangeCase \
    PpChartUnitEffect \
    PpColorSchemeIndex \
    PpDateTimeFormat \
    PpDirection \
    PpEntryEffect \
    PpExportMode \
    PpFarEastLineBreakLevel \
    PpFileDialogType \
    PpFollowColors \
    PpFrameColors \
    PpHTMLVersion \
    PpIndentControl \
    PpMediaType \
    PpMouseActivation \
    PpNumberedBulletStyle \
    PpParagraphAlignment \
    PpPasteDataType \
    PpPlaceholderType \
    PpPrintColorType \
    PpPrintHandoutOrder \
    PpPrintOutputType \
    PpPrintRangeType \
    PpPublishSourceType \
    PpRevisionInfo \
    PpSaveAsFileType \
    PpSelectionType \
    PpShapeFormat \
    PpSlideLayout \
    PpSlideShowAdvanceMode \
    PpSlideShowPointerType \
    PpSlideShowRangeType \
    PpSlideShowState \
    PpSlideShowType \
    PpSlideSizeType \
    PpSoundEffectType \
    PpSoundFormatType \
    PpTabStopType \
    PpTextLevelEffect \
    PpTextStyleType \
    PpTextUnitEffect \
    PpTransitionSpeed \
    PpUpdateOption \
    PpViewType \
    PpWindowState \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/stdole,\
    LoadPictureConstants \
    OLE_TRISTATE \
))
$(eval $(call gb_UnoApi_add_idlfiles,oovbaapi,ooo/vba/word,\
    WdAlertLevel \
    WdAnimation \
    WdArabicNumeral \
    WdAraSpeller \
    WdArrangeStyle \
    WdAutoFitBehavior \
    WdAutoMacros \
    WdAutoVersions \
    WdBaselineAlignment \
    WdBookmarkSortBy \
    WdBorderDistanceFrom \
    WdBorderType \
    WdBorderTypeHID \
    WdBreakType \
    WdBrowserLevel \
    WdBrowseTarget \
    WdBuiltInProperty \
    WdBuiltinStyle \
    WdCalendarType \
    WdCalendarTypeBi \
    WdCaptionLabelID \
    WdCaptionNumberStyle \
    WdCaptionNumberStyleHID \
    WdCaptionPosition \
    WdCellVerticalAlignment \
    WdCharacterCase \
    WdCharacterCaseHID \
    WdCharacterWidth \
    WdChevronConvertRule \
    WdCollapseDirection \
    WdColor \
    WdColorIndex \
    WdCompareTarget \
    WdCompatibility \
    WdConditionCode \
    WdConstants \
    WdContinue \
    WdCountry \
    WdCursorMovement \
    WdCursorType \
    WdCustomLabelPageSize \
    WdDateLanguage \
    WdDefaultFilePath \
    WdDefaultListBehavior \
    WdDefaultTableBehavior \
    WdDeleteCells \
    WdDeletedTextMark \
    WdDiacriticColor \
    WdDictionaryType \
    WdDictionaryTypeHID \
    WdDisableFeaturesIntroducedAfter \
    WdDocumentDirection \
    WdDocumentKind \
    WdDocumentMedium \
    WdDocumentType \
    WdDocumentViewDirection \
    WdDropPosition \
    WdEditionOption \
    WdEditionType \
    WdEditorType \
    WdEmailHTMLFidelity \
    WdEmphasisMark \
    WdEnableCancelKey \
    WdEncloseStyle \
    WdEnclosureType \
    WdEndnoteLocation \
    WdEnvelopeOrientation \
    WdFarEastLineBreakLanguageID \
    WdFarEastLineBreakLevel \
    WdFieldKind \
    WdFieldShading \
    WdFieldType \
    WdFindMatch \
    WdFindWrap \
    WdFlowDirection \
    WdFontBias \
    WdFootnoteLocation \
    WdFramePosition \
    WdFramesetNewFrameLocation \
    WdFramesetSizeType \
    WdFramesetType \
    WdFrameSizeRule \
    WdGoToDirection \
    WdGoToItem \
    WdGutterStyle \
    WdGutterStyleOld \
    WdHeaderFooterIndex \
    WdHeadingSeparator \
    WdHebSpellStart \
    WdHelpType \
    WdHelpTypeHID \
    WdHighAnsiText \
    WdHorizontalInVerticalType \
    WdHorizontalLineAlignment \
    WdHorizontalLineWidthType \
    WdIMEMode \
    WdIndexFilter \
    WdIndexFormat \
    WdIndexSortBy \
    WdIndexType \
    WdInformation \
    WdInlineShapeType \
    WdInsertCells \
    WdInsertedTextMark \
    WdInternationalIndex \
    WdJustificationMode \
    WdKana \
    WdKey \
    WdKeyCategory \
    WdLanguageID \
    WdLanguageID2000 \
    WdLayoutMode \
    WdLetterheadLocation \
    WdLetterStyle \
    WdLineEndingType \
    WdLineSpacing \
    WdLineStyle \
    WdLineType \
    WdLineWidth \
    WdLinkType \
    WdListApplyTo \
    WdListGalleryType \
    WdListLevelAlignment \
    WdListNumberStyle \
    WdListNumberStyleHID \
    WdListType \
    WdMailerPriority \
    WdMailMergeActiveRecord \
    WdMailMergeComparison \
    WdMailMergeDataSource \
    WdMailMergeDefaultRecord \
    WdMailMergeDestination \
    WdMailMergeMailFormat \
    WdMailMergeMainDocType \
    WdMailMergeState \
    WdMailSystem \
    WdMappedDataFields \
    WdMeasurementUnits \
    WdMeasurementUnitsHID \
    WdMergeSubType \
    WdMergeTarget \
    WdMonthNames \
    WdMovementType \
    WdMultipleWordConversionsMode \
    WdNewDocumentType \
    WdNoteNumberStyle \
    WdNoteNumberStyleHID \
    WdNumberingRule \
    WdNumberStyleWordBasicBiDi \
    WdNumberType \
    WdOLEPlacement \
    WdOLEType \
    WdOLEVerb \
    WdOpenFormat \
    WdOrganizerObject \
    WdOrientation \
    WdOriginalFormat \
    WdOutlineLevel \
    WdPageBorderArt \
    WdPageFit \
    WdPageNumberAlignment \
    WdPageNumberStyle \
    WdPageNumberStyleHID \
    WdPaperSize \
    WdPaperTray \
    WdParagraphAlignment \
    WdParagraphAlignmentHID \
    WdPartOfSpeech \
    WdPasteDataType \
    WdPhoneticGuideAlignmentType \
    WdPictureLinkType \
    WdPreferredWidthType \
    WdPrintOutItem \
    WdPrintOutPages \
    WdPrintOutRange \
    WdProofreadingErrorType \
    WdProtectionType \
    WdReadingOrder \
    WdRecoveryType \
    WdRectangleType \
    WdReferenceKind \
    WdReferenceType \
    WdRelativeHorizontalPosition \
    WdRelativeVerticalPosition \
    WdRelocate \
    WdReplace \
    WdRevisedLinesMark \
    WdRevisedPropertiesMark \
    WdRevisionsBalloonMargin \
    WdRevisionsBalloonPrintOrientation \
    WdRevisionsBalloonWidthType \
    WdRevisionsMode \
    WdRevisionsView \
    WdRevisionsWrap \
    WdRevisionType \
    WdRoutingSlipDelivery \
    WdRoutingSlipStatus \
    WdRowAlignment \
    WdRowHeightRule \
    WdRulerStyle \
    WdSalutationGender \
    WdSalutationType \
    WdSaveFormat \
    WdSaveOptions \
    WdScrollbarType \
    WdSectionDirection \
    WdSectionStart \
    WdSeekView \
    WdSelectionFlags \
    WdSelectionType \
    WdSeparatorType \
    WdShapePosition \
    WdShowFilter \
    WdSmartTagControlType \
    WdSortFieldType \
    WdSortFieldTypeHID \
    WdSortOrder \
    WdSortSeparator \
    WdSpecialPane \
    WdSpellingErrorType \
    WdSpellingWordType \
    WdStatistic \
    WdStatisticHID \
    WdStoryType \
    WdStyleSheetLinkType \
    WdStyleSheetPrecedence \
    WdStyleType \
    WdSubscriberFormats \
    WdSummaryLength \
    WdSummaryMode \
    WdTabAlignment \
    WdTabLeader \
    WdTabLeaderHID \
    WdTableDirection \
    WdTableFieldSeparator \
    WdTableFormat \
    WdTableFormatApply \
    WdTablePosition \
    WdTaskPanes \
    WdTCSCConverterDirection \
    WdTemplateType \
    WdTextFormFieldType \
    WdTextOrientation \
    WdTextOrientationHID \
    WdTextureIndex \
    WdToaFormat \
    WdTocFormat \
    WdTofFormat \
    WdTrailingCharacter \
    WdTwoLinesInOneType \
    WdUnderline \
    WdUnits \
    WdUseFormattingFrom \
    WdVerticalAlignment \
    WdViewType \
    WdViewTypeOld \
    WdVisualSelection \
    WdWindowState \
    WdWindowType \
    WdWordDialog \
    WdWordDialogHID \
    WdWordDialogTab \
    WdWordDialogTabHID \
    WdWrapSideType \
    WdWrapType \
    WdWrapTypeMerged \
    WdXMLNodeLevel \
    WdXMLNodeType \
    WdXMLSelectionChangeReason \
    WdXMLValidationStatus \
	XAddin \
	XAddins \
	XApplication \
	XAutoTextEntries \
	XAutoTextEntry \
	XBookmark \
	XBookmarks \
	XBorder \
	XBorders \
	XCell \
	XCells \
	XCheckBox \
	XColumn \
	XColumns \
	XDialog \
	XDialogs \
	XDocument \
	XDocuments \
	XField \
	XFields \
	XFind \
	XFont \
	XFormField \
	XFormFields \
	XFrame \
	XFrames \
	XGlobals \
	XHeaderFooter \
	XHeadersFooters \
	XListFormat \
	XListGalleries \
	XListGallery \
	XListLevel \
	XListLevels \
	XListTemplate \
	XListTemplates \
	XOptions \
	XPageSetup \
	XPane \
	XPanes \
	XParagraph \
	XParagraphFormat \
	XParagraphs \
	XRange \
	XReplacement \
	XRevision \
	XRevisions \
	XRow \
	XRows \
	XSection \
	XSections \
	XSelection \
	XStyle \
	XStyles \
	XSystem \
	XTable \
	XTableOfContents \
	XTables \
	XTablesOfContents \
	XTabStop \
	XTabStops \
	XTemplate \
	XVariable \
	XVariables \
	XView \
	XWindow \
	XWrapFormat \
))

# vim: set noet sw=4 ts=4:
