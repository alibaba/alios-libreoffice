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

#include <java/sql/Ref.hxx>
#include <java/tools.hxx>
using namespace connectivity;

//************ Class: java.sql.Ref


jclass java_sql_Ref::theClass = nullptr;
java_sql_Ref::java_sql_Ref( JNIEnv * pEnv, jobject myObj )
: java_lang_Object( pEnv, myObj )
{
    SDBThreadAttach::addRef();
}
java_sql_Ref::~java_sql_Ref()
{
    SDBThreadAttach::releaseRef();
}

jclass java_sql_Ref::getMyClass() const
{
    // the class must be fetched only once, therefore static
    if( !theClass )
        theClass = findMyClass("java/sql/Ref");
    return theClass;
}

OUString SAL_CALL java_sql_Ref::getBaseTypeName(  )
{
    static jmethodID mID(nullptr);
    return callStringMethod("getBaseTypeName",mID);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
