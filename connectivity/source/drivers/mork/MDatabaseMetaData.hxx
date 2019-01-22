/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_MORK_MDATABASEMETADATA_HXX
#define INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_MORK_MDATABASEMETADATA_HXX

#include <memory>
#include <TDatabaseMetaDataBase.hxx>

namespace connectivity
{
    namespace mork
    {
        class MDatabaseMetaDataHelper;

        //************ Class: ODatabaseMetaData


        class ODatabaseMetaData : public ODatabaseMetaDataBase
        {
            OConnection*               m_pConnection;
            std::unique_ptr<MDatabaseMetaDataHelper>
                                       m_pMetaDataHelper;

            /// @throws css::sdbc::SQLException
            ODatabaseMetaDataResultSet::ORows& SAL_CALL getColumnRows( const OUString& tableNamePattern, const OUString& columnNamePattern );

        protected:
            virtual ~ODatabaseMetaData() override;

        public:
            explicit ODatabaseMetaData(OConnection* _pCon);

        private:
            virtual css::uno::Reference< css::sdbc::XResultSet > impl_getTypeInfo_throw() override;
            // cached database information
            virtual OUString    impl_getIdentifierQuoteString_throw(  ) override;
            virtual bool        impl_isCatalogAtStart_throw(  ) override;
            virtual OUString    impl_getCatalogSeparator_throw(  ) override;
            virtual bool        impl_supportsCatalogsInTableDefinitions_throw(  ) override;
            virtual bool        impl_supportsSchemasInTableDefinitions_throw(  ) override ;
            virtual bool        impl_supportsCatalogsInDataManipulation_throw(  ) override;
            virtual bool        impl_supportsSchemasInDataManipulation_throw(  ) override ;
            virtual bool        impl_supportsMixedCaseQuotedIdentifiers_throw(  ) override ;
            virtual bool        impl_supportsAlterTableWithAddColumn_throw(  ) override;
            virtual bool        impl_supportsAlterTableWithDropColumn_throw(  ) override;
            virtual sal_Int32   impl_getMaxStatements_throw(  ) override;
            virtual sal_Int32   impl_getMaxTablesInSelect_throw(  ) override;
            virtual bool        impl_storesMixedCaseQuotedIdentifiers_throw(  ) override;

            // as I mentioned before this interface is really BIG
            // XDatabaseMetaData
            virtual sal_Bool SAL_CALL allProceduresAreCallable(  ) override;
            virtual sal_Bool SAL_CALL allTablesAreSelectable(  ) override;
            virtual OUString SAL_CALL getURL(  ) override;
            virtual OUString SAL_CALL getUserName(  ) override;
            virtual sal_Bool SAL_CALL isReadOnly(  ) override;
            virtual sal_Bool SAL_CALL nullsAreSortedHigh(  ) override;
            virtual sal_Bool SAL_CALL nullsAreSortedLow(  ) override;
            virtual sal_Bool SAL_CALL nullsAreSortedAtStart(  ) override;
            virtual sal_Bool SAL_CALL nullsAreSortedAtEnd(  ) override;
            virtual OUString SAL_CALL getDatabaseProductName(  ) override;
            virtual OUString SAL_CALL getDatabaseProductVersion(  ) override;
            virtual OUString SAL_CALL getDriverName(  ) override;
            virtual OUString SAL_CALL getDriverVersion(  ) override;
            virtual sal_Int32 SAL_CALL getDriverMajorVersion(  ) override;
            virtual sal_Int32 SAL_CALL getDriverMinorVersion(  ) override;
            virtual sal_Bool SAL_CALL usesLocalFiles(  ) override;
            virtual sal_Bool SAL_CALL usesLocalFilePerTable(  ) override;
            virtual sal_Bool SAL_CALL supportsMixedCaseIdentifiers(  ) override;
            virtual sal_Bool SAL_CALL storesUpperCaseIdentifiers(  ) override;
            virtual sal_Bool SAL_CALL storesLowerCaseIdentifiers(  ) override;
            virtual sal_Bool SAL_CALL storesMixedCaseIdentifiers(  ) override;
            virtual sal_Bool SAL_CALL storesUpperCaseQuotedIdentifiers(  ) override;
            virtual sal_Bool SAL_CALL storesLowerCaseQuotedIdentifiers(  ) override;
            virtual OUString SAL_CALL getSQLKeywords(  ) override;
            virtual OUString SAL_CALL getNumericFunctions(  ) override;
            virtual OUString SAL_CALL getStringFunctions(  ) override;
            virtual OUString SAL_CALL getSystemFunctions(  ) override;
            virtual OUString SAL_CALL getTimeDateFunctions(  ) override;
            virtual OUString SAL_CALL getSearchStringEscape(  ) override;
            virtual OUString SAL_CALL getExtraNameCharacters(  ) override;
            virtual sal_Bool SAL_CALL supportsColumnAliasing(  ) override;
            virtual sal_Bool SAL_CALL nullPlusNonNullIsNull(  ) override;
            virtual sal_Bool SAL_CALL supportsTypeConversion(  ) override;
            virtual sal_Bool SAL_CALL supportsConvert( sal_Int32 fromType, sal_Int32 toType ) override;
            virtual sal_Bool SAL_CALL supportsTableCorrelationNames(  ) override;
            virtual sal_Bool SAL_CALL supportsDifferentTableCorrelationNames(  ) override;
            virtual sal_Bool SAL_CALL supportsExpressionsInOrderBy(  ) override;
            virtual sal_Bool SAL_CALL supportsOrderByUnrelated(  ) override;
            virtual sal_Bool SAL_CALL supportsGroupBy(  ) override;
            virtual sal_Bool SAL_CALL supportsGroupByUnrelated(  ) override;
            virtual sal_Bool SAL_CALL supportsGroupByBeyondSelect(  ) override;
            virtual sal_Bool SAL_CALL supportsLikeEscapeClause(  ) override;
            virtual sal_Bool SAL_CALL supportsMultipleResultSets(  ) override;
            virtual sal_Bool SAL_CALL supportsMultipleTransactions(  ) override;
            virtual sal_Bool SAL_CALL supportsNonNullableColumns(  ) override;
            virtual sal_Bool SAL_CALL supportsMinimumSQLGrammar(  ) override;
            virtual sal_Bool SAL_CALL supportsCoreSQLGrammar(  ) override;
            virtual sal_Bool SAL_CALL supportsExtendedSQLGrammar(  ) override;
            virtual sal_Bool SAL_CALL supportsANSI92EntryLevelSQL(  ) override;
            virtual sal_Bool SAL_CALL supportsANSI92IntermediateSQL(  ) override;
            virtual sal_Bool SAL_CALL supportsANSI92FullSQL(  ) override;
            virtual sal_Bool SAL_CALL supportsIntegrityEnhancementFacility(  ) override;
            virtual sal_Bool SAL_CALL supportsOuterJoins(  ) override;
            virtual sal_Bool SAL_CALL supportsFullOuterJoins(  ) override;
            virtual sal_Bool SAL_CALL supportsLimitedOuterJoins(  ) override;
            virtual OUString SAL_CALL getSchemaTerm(  ) override;
            virtual OUString SAL_CALL getProcedureTerm(  ) override;
            virtual OUString SAL_CALL getCatalogTerm(  ) override;
            virtual sal_Bool SAL_CALL supportsSchemasInProcedureCalls(  ) override;
            virtual sal_Bool SAL_CALL supportsSchemasInIndexDefinitions(  ) override;
            virtual sal_Bool SAL_CALL supportsSchemasInPrivilegeDefinitions(  ) override;
            virtual sal_Bool SAL_CALL supportsCatalogsInProcedureCalls(  ) override;
            virtual sal_Bool SAL_CALL supportsCatalogsInIndexDefinitions(  ) override;
            virtual sal_Bool SAL_CALL supportsCatalogsInPrivilegeDefinitions(  ) override;
            virtual sal_Bool SAL_CALL supportsPositionedDelete(  ) override;
            virtual sal_Bool SAL_CALL supportsPositionedUpdate(  ) override;
            virtual sal_Bool SAL_CALL supportsSelectForUpdate(  ) override;
            virtual sal_Bool SAL_CALL supportsStoredProcedures(  ) override;
            virtual sal_Bool SAL_CALL supportsSubqueriesInComparisons(  ) override;
            virtual sal_Bool SAL_CALL supportsSubqueriesInExists(  ) override;
            virtual sal_Bool SAL_CALL supportsSubqueriesInIns(  ) override;
            virtual sal_Bool SAL_CALL supportsSubqueriesInQuantifieds(  ) override;
            virtual sal_Bool SAL_CALL supportsCorrelatedSubqueries(  ) override;
            virtual sal_Bool SAL_CALL supportsUnion(  ) override;
            virtual sal_Bool SAL_CALL supportsUnionAll(  ) override;
            virtual sal_Bool SAL_CALL supportsOpenCursorsAcrossCommit(  ) override;
            virtual sal_Bool SAL_CALL supportsOpenCursorsAcrossRollback(  ) override;
            virtual sal_Bool SAL_CALL supportsOpenStatementsAcrossCommit(  ) override;
            virtual sal_Bool SAL_CALL supportsOpenStatementsAcrossRollback(  ) override;
            virtual sal_Int32 SAL_CALL getMaxBinaryLiteralLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxCharLiteralLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxColumnNameLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxColumnsInGroupBy(  ) override;
            virtual sal_Int32 SAL_CALL getMaxColumnsInIndex(  ) override;
            virtual sal_Int32 SAL_CALL getMaxColumnsInOrderBy(  ) override;
            virtual sal_Int32 SAL_CALL getMaxColumnsInSelect(  ) override;
            virtual sal_Int32 SAL_CALL getMaxColumnsInTable(  ) override;
            virtual sal_Int32 SAL_CALL getMaxConnections(  ) override;
            virtual sal_Int32 SAL_CALL getMaxCursorNameLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxIndexLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxSchemaNameLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxProcedureNameLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxCatalogNameLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxRowSize(  ) override;
            virtual sal_Bool SAL_CALL doesMaxRowSizeIncludeBlobs(  ) override;
            virtual sal_Int32 SAL_CALL getMaxStatementLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxTableNameLength(  ) override;
            virtual sal_Int32 SAL_CALL getMaxUserNameLength(  ) override;
            virtual sal_Int32 SAL_CALL getDefaultTransactionIsolation(  ) override;
            virtual sal_Bool SAL_CALL supportsTransactions(  ) override;
            virtual sal_Bool SAL_CALL supportsTransactionIsolationLevel( sal_Int32 level ) override;
            virtual sal_Bool SAL_CALL supportsDataDefinitionAndDataManipulationTransactions(  ) override;
            virtual sal_Bool SAL_CALL supportsDataManipulationTransactionsOnly(  ) override;
            virtual sal_Bool SAL_CALL dataDefinitionCausesTransactionCommit(  ) override;
            virtual sal_Bool SAL_CALL dataDefinitionIgnoredInTransactions(  ) override;

            virtual css::uno::Reference< css::sdbc::XResultSet > SAL_CALL getTables( const css::uno::Any& catalog, const OUString& schemaPattern, const OUString& tableNamePattern, const css::uno::Sequence< OUString >& types ) override;
            virtual css::uno::Reference< css::sdbc::XResultSet > SAL_CALL getTableTypes(  ) override;
            virtual css::uno::Reference< css::sdbc::XResultSet > SAL_CALL getTablePrivileges( const css::uno::Any& catalog, const OUString& schemaPattern, const OUString& tableNamePattern ) override;
            virtual css::uno::Reference< css::sdbc::XResultSet > SAL_CALL getColumns( const css::uno::Any& catalog, const OUString& schemaPattern, const OUString& tableNamePattern, const OUString& columnNamePattern ) override;

            virtual sal_Bool SAL_CALL supportsResultSetType( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL supportsResultSetConcurrency( sal_Int32 setType, sal_Int32 concurrency ) override;
            virtual sal_Bool SAL_CALL ownUpdatesAreVisible( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL ownDeletesAreVisible( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL ownInsertsAreVisible( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL othersUpdatesAreVisible( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL othersDeletesAreVisible( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL othersInsertsAreVisible( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL updatesAreDetected( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL deletesAreDetected( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL insertsAreDetected( sal_Int32 setType ) override;
            virtual sal_Bool SAL_CALL supportsBatchUpdates(  ) override;

            virtual css::uno::Reference< css::sdbc::XResultSet > SAL_CALL getUDTs( const css::uno::Any& catalog, const OUString& schemaPattern, const OUString& typeNamePattern, const css::uno::Sequence< sal_Int32 >& types ) override;
        };
    }
}

#endif // INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_MORK_MDATABASEMETADATA_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
