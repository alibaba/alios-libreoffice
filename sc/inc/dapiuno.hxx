/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_SC_INC_DAPIUNO_HXX
#define INCLUDED_SC_INC_DAPIUNO_HXX

#include "global.hxx"
#include "dpobject.hxx"
#include "rangeutl.hxx"
#include "cellsuno.hxx"

#include <svl/lstner.hxx>
#include <svl/itemprop.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>

#include <com/sun/star/sheet/DataPilotFieldAutoShowInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldGroupInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldLayoutInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>
#include <com/sun/star/sheet/DataPilotFieldReference.hpp>
#include <com/sun/star/sheet/DataPilotFieldSortInfo.hpp>
#include <com/sun/star/sheet/XDataPilotDataLayoutFieldSupplier.hpp>
#include <com/sun/star/sheet/XDataPilotDescriptor.hpp>
#include <com/sun/star/sheet/XDataPilotField.hpp>
#include <com/sun/star/sheet/XDataPilotFieldGrouping.hpp>
#include <com/sun/star/sheet/XDataPilotTable.hpp>
#include <com/sun/star/sheet/XDataPilotTable2.hpp>
#include <com/sun/star/sheet/XDataPilotTables.hpp>

#include <cppuhelper/implbase.hxx>

#include <memory>
#include <vector>

namespace com { namespace sun { namespace star { namespace sheet {
    struct DataPilotFieldFilter;
    struct DataPilotTablePositionData;
}}}}

class ScDocShell;
class ScDPSaveDimension;
struct ScDPNumGroupInfo;

class ScDataPilotTableObj;
class ScDataPilotFieldObj;
class ScDataPilotItemObj;
enum class ScGeneralFunction;

class ScDataPilotConversion
{
public:
    static ScGeneralFunction   FirstFunc( PivotFunc nBits );
    static PivotFunc           FunctionBit( sal_Int16 eFunc );

    static void         FillGroupInfo(
                            css::sheet::DataPilotFieldGroupInfo& rInfo,
                            const ScDPNumGroupInfo& rGroupInfo );
};

/** DataPilotTables collection per sheet. */
class ScDataPilotTablesObj : public cppu::WeakImplHelper<
                                        css::sheet::XDataPilotTables,
                                        css::container::XEnumerationAccess,
                                        css::container::XIndexAccess,
                                        css::lang::XServiceInfo>,
                                    public SfxListener
{
private:
    ScDocShell*             pDocShell;
    SCTAB                   nTab;

    ScDataPilotTableObj*    GetObjectByIndex_Impl( sal_Int32 nIndex );
    ScDataPilotTableObj*    GetObjectByName_Impl(const OUString& aName);

public:
                            ScDataPilotTablesObj(ScDocShell* pDocSh, SCTAB nT);
    virtual                 ~ScDataPilotTablesObj() override;

    virtual void            Notify( SfxBroadcaster& rBC, const SfxHint& rHint ) override;

                            // XDataPilotTables
    virtual css::uno::Reference< css::sheet::XDataPilotDescriptor > SAL_CALL
                            createDataPilotDescriptor() override;
    virtual void SAL_CALL   insertNewByName( const OUString& aName,
                                    const css::table::CellAddress& aOutputAddress,
                                    const css::uno::Reference< css::sheet::XDataPilotDescriptor >& xDescriptor ) override;
    virtual void SAL_CALL   removeByName( const OUString& aName ) override;

                            // XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const OUString& aName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getElementNames() override;
    virtual sal_Bool SAL_CALL hasByName( const OUString& aName ) override;

                            // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() override;
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index ) override;

                            // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL
                            createEnumeration() override;

                            // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() override;
    virtual sal_Bool SAL_CALL hasElements() override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;
};

//  ScDataPilotDescriptorBase is never instantiated directly
class ScDataPilotDescriptorBase : public css::sheet::XDataPilotDescriptor,
                                  public css::beans::XPropertySet,
                                  public css::sheet::XDataPilotDataLayoutFieldSupplier,
                                  public css::lang::XServiceInfo,
                                  public css::lang::XUnoTunnel,
                                  public css::lang::XTypeProvider,
                                  public cppu::OWeakObject,
                                  public SfxListener
{
private:
    SfxItemPropertySet  maPropSet;
    ScDocShell*         pDocShell;

public:
                            ScDataPilotDescriptorBase(ScDocShell* pDocSh);
    virtual                 ~ScDataPilotDescriptorBase() override;

    virtual css::uno::Any SAL_CALL queryInterface(
                                const css::uno::Type & rType ) override;
    virtual void SAL_CALL   acquire() throw() override;
    virtual void SAL_CALL   release() throw() override;

    virtual void            Notify( SfxBroadcaster& rBC, const SfxHint& rHint ) override;

    ScDocShell*             GetDocShell() const     { return pDocShell; }

                            // in the derivatives:
    virtual ScDPObject* GetDPObject() const = 0;
    virtual void SetDPObject(ScDPObject* pDPObj) = 0;

                            // XDataPilotDescriptor
                            //  getName, setName, getTag, setTag in derived classes

    virtual css::table::CellRangeAddress SAL_CALL getSourceRange() override;
    virtual void SAL_CALL setSourceRange( const css::table::CellRangeAddress& aSourceRange ) override;
    virtual css::uno::Reference< css::sheet::XSheetFilterDescriptor > SAL_CALL
                            getFilterDescriptor() override;
    virtual css::uno::Reference< css::container::XIndexAccess > SAL_CALL
                            getDataPilotFields() override;
    virtual css::uno::Reference< css::container::XIndexAccess > SAL_CALL
                            getColumnFields() override;
    virtual css::uno::Reference< css::container::XIndexAccess > SAL_CALL
                            getRowFields() override;
    virtual css::uno::Reference< css::container::XIndexAccess > SAL_CALL
                            getPageFields() override;
    virtual css::uno::Reference< css::container::XIndexAccess > SAL_CALL
                            getDataFields() override;
    virtual css::uno::Reference< css::container::XIndexAccess > SAL_CALL
                            getHiddenFields() override;

                            // XPropertySet
    virtual css::uno::Reference< css::beans::XPropertySetInfo >
                            SAL_CALL getPropertySetInfo(  ) override;
    virtual void SAL_CALL   setPropertyValue( const OUString& aPropertyName,
                                    const css::uno::Any& aValue ) override;
    virtual css::uno::Any SAL_CALL getPropertyValue(
                                    const OUString& PropertyName ) override;
    virtual void SAL_CALL   addPropertyChangeListener( const OUString& aPropertyName,
                                    const css::uno::Reference< css::beans::XPropertyChangeListener >& xListener ) override;
    virtual void SAL_CALL   removePropertyChangeListener( const OUString& aPropertyName,
                                    const css::uno::Reference< css::beans::XPropertyChangeListener >& aListener ) override;
    virtual void SAL_CALL   addVetoableChangeListener( const OUString& PropertyName,
                                    const css::uno::Reference< css::beans::XVetoableChangeListener >& aListener ) override;
    virtual void SAL_CALL   removeVetoableChangeListener( const OUString& PropertyName,
                                    const css::uno::Reference< css::beans::XVetoableChangeListener >& aListener ) override;

                            // XDataPilotDataLayoutFieldSupplier
    virtual css::uno::Reference< css::sheet::XDataPilotField >
                            SAL_CALL getDataLayoutField() override;

                            // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const css::uno::Sequence<
                                    sal_Int8 >& aIdentifier ) override;

    static const css::uno::Sequence<sal_Int8>& getUnoTunnelId();
    SC_DLLPUBLIC static ScDataPilotDescriptorBase* getImplementation(const css::uno::Reference<css::sheet::XDataPilotDescriptor>& rObj);

                            // XTypeProvider (override in ScDataPilotTableObj)
    virtual css::uno::Sequence< css::uno::Type > SAL_CALL getTypes() override;
    virtual css::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() override;

                            // XServiceInfo is in derived classes
};

class ScDataPilotDescriptor : public ScDataPilotDescriptorBase
{
private:
    std::unique_ptr<ScDPObject>  mpDPObject;

public:
                            ScDataPilotDescriptor(ScDocShell* pDocSh);
    virtual                 ~ScDataPilotDescriptor() override;

    virtual ScDPObject* GetDPObject() const override;
    virtual void SetDPObject(ScDPObject* pDPObj) override;

                            // rest of XDataPilotDescriptor (incl. XNamed)
    virtual OUString SAL_CALL getName() override;
    virtual void SAL_CALL   setName( const OUString& aName ) override;
    virtual OUString SAL_CALL getTag() override;
    virtual void SAL_CALL   setTag( const OUString& aTag ) override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;
};

class ScDataPilotTableObj : public ScDataPilotDescriptorBase,
                            public css::sheet::XDataPilotTable2,
                            public css::util::XModifyBroadcaster
{
private:
    SCTAB                   nTab;
    OUString                aName;
    XModifyListenerArr_Impl aModifyListeners;

    void                    Refreshed_Impl();

public:
                            ScDataPilotTableObj(ScDocShell* pDocSh, SCTAB nT, const OUString& rN);
    virtual                 ~ScDataPilotTableObj() override;

    virtual css::uno::Any SAL_CALL queryInterface( const css::uno::Type & rType ) override;
    virtual void SAL_CALL   acquire() throw() override;
    virtual void SAL_CALL   release() throw() override;

    virtual void            Notify( SfxBroadcaster& rBC, const SfxHint& rHint ) override;

    virtual ScDPObject* GetDPObject() const override;
    virtual void SetDPObject(ScDPObject* pDPObj) override;

                            // rest of XDataPilotDescriptor (incl. XNamed)
    virtual OUString SAL_CALL getName() override;
    virtual void SAL_CALL   setName( const OUString& aName ) override;
    virtual OUString SAL_CALL getTag() override;
    virtual void SAL_CALL   setTag( const OUString& aTag ) override;

                            // XDataPilotTable
    virtual css::table::CellRangeAddress SAL_CALL getOutputRange() override;
    virtual void SAL_CALL   refresh() override;

                            // XDataPilotTable2
    virtual css::uno::Sequence< css::uno::Sequence< css::uno::Any > >
        SAL_CALL getDrillDownData(const css::table::CellAddress& aAddr) override;

    virtual css::sheet::DataPilotTablePositionData
        SAL_CALL getPositionData(const css::table::CellAddress& aAddr) override;

    virtual void SAL_CALL insertDrillDownSheet(const css::table::CellAddress& aAddr) override;

    virtual css::table::CellRangeAddress SAL_CALL getOutputRangeByType( sal_Int32 nType ) override;

                            // XModifyBroadcaster
    virtual void SAL_CALL   addModifyListener( const css::uno::Reference< css::util::XModifyListener >& aListener ) override;
    virtual void SAL_CALL   removeModifyListener( const css::uno::Reference< css::util::XModifyListener >& aListener ) override;

                            // XTypeProvider (override)
    virtual css::uno::Sequence< css::uno::Type > SAL_CALL getTypes() override;
    virtual css::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;
};

struct ScFieldIdentifier
{
    OUString            maFieldName;    /// Source field name.
    sal_Int32           mnFieldIdx;     /// Field index (if several fields with same name exist).
    bool                mbDataLayout;   /// True = data layout field collecting all data fields as items.

    explicit     ScFieldIdentifier() :
                            mnFieldIdx( 0 ), mbDataLayout( false ) {}

    explicit     ScFieldIdentifier( const OUString& rFieldName, bool bDataLayout ) :
                            maFieldName( rFieldName ), mnFieldIdx( 0 ), mbDataLayout( bDataLayout ) {}
};

/** Base class of all implementation objects based on a DataPilot descriptor
    or DataPilot table object. Wraps acquiring and releasing the parent. */
class ScDataPilotChildObjBase
{
protected:
    explicit            ScDataPilotChildObjBase( ScDataPilotDescriptorBase& rParent );
    explicit            ScDataPilotChildObjBase( ScDataPilotDescriptorBase& rParent, const ScFieldIdentifier& rFieldId );
    virtual             ~ScDataPilotChildObjBase();

    /** Returns the wrapped DataPilot object (calls GetDPObject() at parent). */
    ScDPObject*         GetDPObject() const;
    /** Sets the passed DataPilot object (calls SetDPObject() at parent). */
    void                SetDPObject( ScDPObject* pDPObject );

    /** Returns the DataPilot dimension object related to the field described by maFieldId. */
    ScDPSaveDimension*  GetDPDimension( ScDPObject** ppDPObject = nullptr ) const;

    /** Returns the number of members for the field described by maFieldId. */
    sal_Int32           GetMemberCount() const;
    /** Returns the collection of members for the field described by maFieldId. */
    css::uno::Reference< css::sheet::XMembersAccess > GetMembers() const;

    ScDocShell* GetDocShell() const;
protected:
    rtl::Reference<ScDataPilotDescriptorBase> mxParent;
    ScFieldIdentifier   maFieldId;

private:
    ScDataPilotChildObjBase& operator=( const ScDataPilotChildObjBase& ) = delete;
};

typedef ::cppu::WeakImplHelper
<
    css::container::XEnumerationAccess,
    css::container::XIndexAccess,
    css::container::XNameAccess,
    css::lang::XServiceInfo
>
ScDataPilotFieldsObjImpl;

/** Collection of all DataPilot fields, or of all fields from a specific dimension. */
class ScDataPilotFieldsObj : public ScDataPilotChildObjBase, public ScDataPilotFieldsObjImpl
{
public:
    explicit            ScDataPilotFieldsObj(
                            ScDataPilotDescriptorBase& rParent );

    explicit            ScDataPilotFieldsObj(
                            ScDataPilotDescriptorBase& rParent,
                            css::sheet::DataPilotFieldOrientation eOrient );

    virtual             ~ScDataPilotFieldsObj() override;

                            // XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const OUString& aName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getElementNames() override;
    virtual sal_Bool SAL_CALL hasByName( const OUString& aName ) override;

                            // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() override;
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index ) override;

                            // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL
                            createEnumeration() override;

                            // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() override;
    virtual sal_Bool SAL_CALL hasElements() override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;

private:
    ScDataPilotFieldObj*    GetObjectByIndex_Impl( sal_Int32 nIndex ) const;
    ScDataPilotFieldObj*    GetObjectByName_Impl( const OUString& rName ) const;

private:
    css::uno::Any maOrient;    /// Field orientation, no value = all fields.
};

typedef ::cppu::WeakImplHelper
<
    css::container::XNamed,
    css::beans::XPropertySet,
    css::sheet::XDataPilotField,
    css::sheet::XDataPilotFieldGrouping,
    css::lang::XServiceInfo
>
ScDataPilotFieldObjImpl;

/** Implementation of a single DataPilot field. */
class ScDataPilotFieldObj : public ScDataPilotChildObjBase, public ScDataPilotFieldObjImpl
{
public:
                        ScDataPilotFieldObj(
                            ScDataPilotDescriptorBase& rParent,
                            const ScFieldIdentifier& rIdent );

                        ScDataPilotFieldObj(
                            ScDataPilotDescriptorBase& rParent,
                            const ScFieldIdentifier& rIdent,
                            const css::uno::Any& rOrient );

    virtual             ~ScDataPilotFieldObj() override;

                            // XNamed
    virtual OUString SAL_CALL getName() override;
    virtual void SAL_CALL   setName(const OUString& aName) override;

                            // XPropertySet
    virtual css::uno::Reference< css::beans::XPropertySetInfo >
                            SAL_CALL getPropertySetInfo(  ) override;
    virtual void SAL_CALL   setPropertyValue( const OUString& aPropertyName,
                                    const css::uno::Any& aValue ) override;
    virtual css::uno::Any SAL_CALL getPropertyValue(const OUString& PropertyName ) override;
    virtual void SAL_CALL   addPropertyChangeListener( const OUString& aPropertyName,
                                    const css::uno::Reference< css::beans::XPropertyChangeListener >& xListener ) override;
    virtual void SAL_CALL   removePropertyChangeListener( const OUString& aPropertyName,
                                    const css::uno::Reference< css::beans::XPropertyChangeListener >& aListener ) override;
    virtual void SAL_CALL   addVetoableChangeListener( const OUString& PropertyName,
                                    const css::uno::Reference< css::beans::XVetoableChangeListener >& aListener ) override;
    virtual void SAL_CALL   removeVetoableChangeListener( const OUString& PropertyName,
                                    const css::uno::Reference< css::beans::XVetoableChangeListener >& aListener ) override;

                            // XDatePilotField
    virtual css::uno::Reference<css::container::XIndexAccess> SAL_CALL
                                getItems() override;

                            // only called from property-functions:
    css::sheet::DataPilotFieldOrientation getOrientation() const;
    void setOrientation(css::sheet::DataPilotFieldOrientation Orientation);
    sal_Int16 getFunction() const;
    void setFunction(ScGeneralFunction Function);
    css::uno::Sequence< sal_Int16 > getSubtotals() const;
    void setSubtotals(const std::vector< ScGeneralFunction >& rFunctions);
    void setCurrentPage(const OUString& sPage);
    void setUseCurrentPage(bool bUse);
    const css::sheet::DataPilotFieldAutoShowInfo* getAutoShowInfo();
    void setAutoShowInfo(const css::sheet::DataPilotFieldAutoShowInfo* pInfo);
    const css::sheet::DataPilotFieldLayoutInfo* getLayoutInfo();
    void setLayoutInfo(const css::sheet::DataPilotFieldLayoutInfo* pInfo);
    const css::sheet::DataPilotFieldReference* getReference();
    void setReference(const css::sheet::DataPilotFieldReference* pInfo);
    const css::sheet::DataPilotFieldSortInfo* getSortInfo();
    void setSortInfo(const css::sheet::DataPilotFieldSortInfo* pInfo);
    bool getShowEmpty() const;
    void setShowEmpty(bool bShow);
    bool getRepeatItemLabels() const;
    void setRepeatItemLabels(bool bShow);

    bool hasGroupInfo();
    css::sheet::DataPilotFieldGroupInfo getGroupInfo();
    void setGroupInfo(const css::sheet::DataPilotFieldGroupInfo* pInfo);

                            // XDataPilotFieldGrouping
    static bool HasString(const css::uno::Sequence< OUString >& aItems, const OUString& aString);
    virtual css::uno::Reference < css::sheet::XDataPilotField > SAL_CALL
        createNameGroup(const css::uno::Sequence< OUString >& aItems) override;
    virtual css::uno::Reference < css::sheet::XDataPilotField > SAL_CALL
        createDateGroup(const css::sheet::DataPilotFieldGroupInfo& rInfo) override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;

private:
    css::uno::Reference< css::container::XIndexAccess >
                        mxItems;
    SfxItemPropertySet  maPropSet;
    css::uno::Any maOrient;
};

typedef ::std::vector< OUString > ScFieldGroupMembers;

struct ScFieldGroup
{
    OUString     maName;
    ScFieldGroupMembers maMembers;
};

typedef ::std::vector< ScFieldGroup > ScFieldGroups;

typedef ::cppu::WeakImplHelper
<
    css::container::XNameContainer,
    css::container::XEnumerationAccess,
    css::container::XIndexAccess,
    css::lang::XServiceInfo
>
ScDataPilotFieldGroupsObjImpl;

/** Implementation of all grouped items in a DataPilot field.

    This is a stand-alone object without any connection to the base DataPilot
    field. Grouping info has to be written back with the GroupInfo property of
    the DataPilot field after modifying this object.
 */
class ScDataPilotFieldGroupsObj : public ScDataPilotFieldGroupsObjImpl
{
public:
    explicit            ScDataPilotFieldGroupsObj( const ScFieldGroups& rGroups );
    virtual             ~ScDataPilotFieldGroupsObj() override;

                            // XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const OUString& aName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getElementNames() override;
    virtual sal_Bool SAL_CALL hasByName( const OUString& aName ) override;

                                // XNameReplace
    virtual void SAL_CALL replaceByName( const OUString& aName,
                                const css::uno::Any& aElement ) override;

                            // XNameContainer
    virtual void SAL_CALL insertByName( const OUString& aName,
                                const css::uno::Any& aElement ) override;
    virtual void SAL_CALL removeByName( const OUString& Name ) override;

                        // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() override;
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index ) override;

                            // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL
                            createEnumeration() override;

                            // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() override;
    virtual sal_Bool SAL_CALL hasElements() override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;

    // implementation
    /// @throws css::uno::RuntimeException
    ScFieldGroup&       getFieldGroup( const OUString& rName );
    /// @throws css::uno::RuntimeException
    void                renameFieldGroup( const OUString& rOldName, const OUString& rNewName );

private:
    ScFieldGroups::iterator implFindByName( const OUString& rName );

private:
    ScFieldGroups       maGroups;
};

typedef ::cppu::WeakImplHelper
<
    css::container::XNameContainer,
    css::container::XEnumerationAccess,
    css::container::XIndexAccess,
    css::container::XNamed,
    css::lang::XServiceInfo
>
ScDataPilotFieldGroupObjImpl;

class ScDataPilotFieldGroupObj : public ScDataPilotFieldGroupObjImpl
{
public:
    explicit            ScDataPilotFieldGroupObj( ScDataPilotFieldGroupsObj& rParent, const OUString& rGroupName );
    virtual             ~ScDataPilotFieldGroupObj() override;

                            // XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const OUString& aName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getElementNames() override;
    virtual sal_Bool SAL_CALL hasByName( const OUString& aName ) override;

                                // XNameReplace
    virtual void SAL_CALL replaceByName( const OUString& aName,
                                const css::uno::Any& aElement ) override;

                                // XNameContainer
    virtual void SAL_CALL insertByName( const OUString& aName,
                                const css::uno::Any& aElement ) override;
    virtual void SAL_CALL removeByName( const OUString& Name ) override;

                    // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() override;
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index ) override;

                            // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL
                            createEnumeration() override;

                            // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() override;
    virtual sal_Bool SAL_CALL hasElements() override;

                            // XNamed
    virtual OUString SAL_CALL getName() override;
    virtual void SAL_CALL   setName( const OUString& aName ) override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;

private:
    rtl::Reference<ScDataPilotFieldGroupsObj> mxParent;
    OUString     maGroupName;
};

typedef ::cppu::WeakImplHelper
<
    css::container::XNamed,
    css::lang::XServiceInfo
>
ScDataPilotFieldGroupItemObjImpl;

class ScDataPilotFieldGroupItemObj : public ScDataPilotFieldGroupItemObjImpl
{
public:
    explicit            ScDataPilotFieldGroupItemObj( ScDataPilotFieldGroupObj& rParent, const OUString& rName );
    virtual             ~ScDataPilotFieldGroupItemObj() override;

                            // XNamed
    virtual OUString SAL_CALL getName() override;
    virtual void SAL_CALL   setName( const OUString& aName ) override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;

private:
    rtl::Reference<ScDataPilotFieldGroupObj> mxParent;
    OUString     maName;
};

typedef ::cppu::WeakImplHelper
<
    css::container::XEnumerationAccess,
    css::container::XIndexAccess,
    css::container::XNameAccess,
    css::lang::XServiceInfo
>
ScDataPilotItemsObjImpl;

class ScDataPilotItemsObj : public ScDataPilotChildObjBase, public ScDataPilotItemsObjImpl
{
public:
    explicit            ScDataPilotItemsObj( ScDataPilotDescriptorBase& rParent, const ScFieldIdentifier& rFieldId );
    virtual             ~ScDataPilotItemsObj() override;

                            // XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const OUString& aName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getElementNames() override;
    virtual sal_Bool SAL_CALL hasByName( const OUString& aName ) override;

                            // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() override;
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index ) override;

                            // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL
                            createEnumeration() override;

                            // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() override;
    virtual sal_Bool SAL_CALL hasElements() override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;

private:
    ScDataPilotItemObj* GetObjectByIndex_Impl( sal_Int32 nIndex ) const;
};

typedef ::cppu::WeakImplHelper
<
    css::container::XNamed,
    css::beans::XPropertySet,
    css::lang::XServiceInfo
>
ScDataPilotItemObjImpl;

class ScDataPilotItemObj : public ScDataPilotChildObjBase, public ScDataPilotItemObjImpl
{
public:
    explicit            ScDataPilotItemObj(
                            ScDataPilotDescriptorBase& rParent,
                            const ScFieldIdentifier& rFieldId,
                            sal_Int32 nIndex );

    virtual             ~ScDataPilotItemObj() override;

                            // XNamed
    virtual OUString SAL_CALL getName() override;
    virtual void SAL_CALL   setName( const OUString& aName ) override;

                            // XPropertySet
    virtual css::uno::Reference< css::beans::XPropertySetInfo >
                            SAL_CALL getPropertySetInfo(  ) override;
    virtual void SAL_CALL   setPropertyValue( const OUString& aPropertyName,
                                    const css::uno::Any& aValue ) override;
    virtual css::uno::Any SAL_CALL getPropertyValue(
                                    const OUString& PropertyName ) override;
    virtual void SAL_CALL   addPropertyChangeListener( const OUString& aPropertyName,
                                    const css::uno::Reference< css::beans::XPropertyChangeListener >& xListener ) override;
    virtual void SAL_CALL   removePropertyChangeListener( const OUString& aPropertyName,
                                    const css::uno::Reference< css::beans::XPropertyChangeListener >& aListener ) override;
    virtual void SAL_CALL   addVetoableChangeListener( const OUString& PropertyName,
                                    const css::uno::Reference< css::beans::XVetoableChangeListener >& aListener ) override;
    virtual void SAL_CALL   removeVetoableChangeListener( const OUString& PropertyName,
                                    const css::uno::Reference< css::beans::XVetoableChangeListener >& aListener ) override;

                            // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() override;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) override;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override;

private:
    SfxItemPropertySet  maPropSet;
    sal_Int32           mnIndex;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
