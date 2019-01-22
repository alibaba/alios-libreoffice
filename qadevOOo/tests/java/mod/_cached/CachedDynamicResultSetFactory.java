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

package mod._cached;

import com.sun.star.beans.Property;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.ucb.Command;
import com.sun.star.ucb.NumberedSortingInfo;
import com.sun.star.ucb.OpenCommandArgument2;
import com.sun.star.ucb.OpenMode;
import com.sun.star.ucb.XCachedDynamicResultSetStubFactory;
import com.sun.star.ucb.XCommandProcessor;
import com.sun.star.ucb.XContent;
import com.sun.star.ucb.XContentIdentifier;
import com.sun.star.ucb.XContentIdentifierFactory;
import com.sun.star.ucb.XContentProvider;
import com.sun.star.ucb.XDynamicResultSet;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.Type;
import com.sun.star.uno.AnyConverter;
import java.io.PrintWriter;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

/**
* Test for object which is represented by service
* <code>com.sun.star.ucb.CachedDynamicResultSetFactory</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::ucb::XCachedDynamicResultSetFactory</code></li>
* </ul> <p>
* This object test <b> is NOT </b> designed to be run in several
* threads concurrently.
* @see com.sun.star.ucb.XCachedDynamicResultSetFactory
* @see com.sun.star.ucb.CachedDynamicResultSetFactory
* @see ifc.ucb._XCachedDynamicResultSetFactory
*/
public class CachedDynamicResultSetFactory extends TestCase {

    /**
    * Creating a TestEnvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.ucb.CachedDynamicResultSetFactory</code>. <p>
    *     Object relations created :
    * <ul>
    *  <li> <code>'CachedDynamicResultSetStub'</code> for
    *      {@link ifc.ucb._XCachedDynamicResultSetFactory} : the destination
    *   interface requires as its parameter an instance of
    *   <code>CachedDynamicResultSetStub</code> service. It is created
    *   using <code>UniversalContentBroker</code> and querying it for
    *   <code>PackageContent</code> which represents JAR file mentioned
    *   above. Then the dynamic list of file contents (entries) is retrieved.
    *   Using <code>CachedDynamicResultSetStubFactory</code> service a stub of
    *   Cached result set is created which is relation required.
    *  </li>
    * </ul>
    */
    @Override
    public TestEnvironment createTestEnvironment( TestParameters Param,
                                                  PrintWriter log )
                                                    throws Exception {
        XMultiServiceFactory xMSF = Param.getMSF();
        Object oInterface = xMSF.createInstance
                ( "com.sun.star.ucb.CachedDynamicResultSetFactory" );
        XInterface oObj = (XInterface) oInterface;

        TestEnvironment tEnv = new TestEnvironment( oObj );

        // creating relation for XCachedDynamicResultSetFactory
        XDynamicResultSet resSetStub = null ;
        Object oUCB = xMSF.createInstanceWithArguments
            ("com.sun.star.ucb.UniversalContentBroker",
            new Object[0]) ;

        XContentIdentifierFactory ciFac = UnoRuntime.queryInterface(XContentIdentifierFactory.class, oUCB) ;

        String url = util.utils.getFullTestURL("SwXTextEmbeddedObject.sxw") ;
        StringBuilder escUrl = new StringBuilder();

        // In base URL of a JAR file in content URL all directory
        // separators ('/') must be replaced with escape symbol '%2F'.
        int idx = url.indexOf("/") ;
        int lastIdx = -1 ;
        while (idx >= 0) {
            escUrl.append(url.substring(lastIdx + 1, idx)).append("%2F");
            lastIdx = idx ;
            idx = url.indexOf("/", idx + 1) ;
        }
        escUrl.append(url.substring(lastIdx + 1));
        String cntUrl = "vnd.sun.star.pkg://" + escUrl.toString() + "/" ;

        XContentIdentifier CI = ciFac.createContentIdentifier(cntUrl) ;

        XContentProvider cntProv = UnoRuntime.queryInterface(XContentProvider.class, oUCB) ;

        XContent cnt = cntProv.queryContent(CI) ;

        XCommandProcessor cmdProc = UnoRuntime.queryInterface(XCommandProcessor.class, cnt) ;

        Property prop = new Property() ;
        prop.Name = "Title" ;

        Command cmd = new Command("open", -1, new OpenCommandArgument2
            (OpenMode.ALL, 10000, null, new Property[] {prop},
             new NumberedSortingInfo[0])) ;

        XDynamicResultSet dynResSet = (XDynamicResultSet)
                AnyConverter.toObject(new Type(XDynamicResultSet.class),
                                    cmdProc.execute(cmd, 0, null));

        Object oStubFactory = xMSF.createInstance
            ("com.sun.star.ucb.CachedDynamicResultSetStubFactory") ;

        XCachedDynamicResultSetStubFactory xStubFactory =
            UnoRuntime.queryInterface
        (XCachedDynamicResultSetStubFactory.class, oStubFactory) ;

        resSetStub =
            xStubFactory.createCachedDynamicResultSetStub(dynResSet) ;

        tEnv.addObjRelation("CachedDynamicResultSetStub", resSetStub) ;

        return tEnv;
    } // finish method getTestEnvironment

}

