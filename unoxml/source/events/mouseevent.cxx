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

#include <mouseevent.hxx>

using namespace css::uno;
using namespace css::xml::dom::events;
using namespace css::xml::dom::views;

namespace DOM { namespace events
{
    CMouseEvent::CMouseEvent()
        : CMouseEvent_Base()
        , m_screenX(0)
        , m_screenY(0)
        , m_clientX(0)
        , m_clientY(0)
        , m_ctrlKey(false)
        , m_shiftKey(false)
        , m_altKey(false)
        , m_metaKey(false)
        , m_button(0)
    {
    }

    sal_Int32 SAL_CALL CMouseEvent::getScreenX()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_screenX;
    }
    sal_Int32 SAL_CALL CMouseEvent::getScreenY()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_screenY;
    }
    sal_Int32 SAL_CALL CMouseEvent::getClientX()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_clientX;
    }
    sal_Int32 SAL_CALL CMouseEvent::getClientY()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_clientY;
    }
    sal_Bool SAL_CALL CMouseEvent::getCtrlKey()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_ctrlKey;
    }
    sal_Bool SAL_CALL CMouseEvent::getShiftKey()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_shiftKey;
    }
    sal_Bool SAL_CALL CMouseEvent::getAltKey()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_altKey;
    }
    sal_Bool SAL_CALL CMouseEvent::getMetaKey()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_metaKey;
    }
    sal_Int16 SAL_CALL CMouseEvent::getButton()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_button;
    }
    Reference< XEventTarget > SAL_CALL CMouseEvent::getRelatedTarget()
    {
        return Reference< XEventTarget >();
    }

    void SAL_CALL CMouseEvent::initMouseEvent(
                        const OUString& typeArg,
                        sal_Bool canBubbleArg,
                        sal_Bool cancelableArg,
                        const Reference< XAbstractView >& viewArg,
                        sal_Int32 detailArg,
                        sal_Int32 screenXArg,
                        sal_Int32 screenYArg,
                        sal_Int32 clientXArg,
                        sal_Int32 clientYArg,
                        sal_Bool ctrlKeyArg,
                        sal_Bool altKeyArg,
                        sal_Bool shiftKeyArg,
                        sal_Bool metaKeyArg,
                        sal_Int16 buttonArg,
                        const Reference< XEventTarget >& /*relatedTargetArg*/)
    {
        ::osl::MutexGuard const g(m_Mutex);

        CUIEvent::initUIEvent(typeArg, canBubbleArg, cancelableArg, viewArg, detailArg);
        m_screenX = screenXArg;
        m_screenY = screenYArg;
        m_clientX = clientXArg;
        m_clientY = clientYArg;
        m_ctrlKey = ctrlKeyArg;
        m_altKey = altKeyArg;
        m_shiftKey = shiftKeyArg;
        m_metaKey = metaKeyArg;
        m_button = buttonArg;
    }

    // delegate to CUIEvent, since we are inheriting from CUIEvent and XUIEvent
    Reference< XAbstractView > SAL_CALL CMouseEvent::getView()
    {
        return CUIEvent::getView();
    }

    sal_Int32 SAL_CALL CMouseEvent::getDetail()
    {
        return CUIEvent::getDetail();
    }

    void SAL_CALL CMouseEvent::initUIEvent(const OUString& typeArg,
                     sal_Bool canBubbleArg,
                     sal_Bool cancelableArg,
                     const Reference< XAbstractView >& viewArg,
                     sal_Int32 detailArg)
    {
        CUIEvent::initUIEvent(typeArg, canBubbleArg, cancelableArg, viewArg, detailArg);
    }

    OUString SAL_CALL CMouseEvent::getType()
    {
        return CUIEvent::getType();
    }

    Reference< XEventTarget > SAL_CALL CMouseEvent::getTarget()
    {
        return CUIEvent::getTarget();
    }

    Reference< XEventTarget > SAL_CALL CMouseEvent::getCurrentTarget()
    {
        return CUIEvent::getCurrentTarget();
    }

    PhaseType SAL_CALL CMouseEvent::getEventPhase()
    {
        return CUIEvent::getEventPhase();
    }

    sal_Bool SAL_CALL CMouseEvent::getBubbles()
    {
        return CEvent::getBubbles();
    }

    sal_Bool SAL_CALL CMouseEvent::getCancelable()
    {
        return CUIEvent::getCancelable();
    }

    css::util::Time SAL_CALL CMouseEvent::getTimeStamp()
    {
        return CUIEvent::getTimeStamp();
    }

    void SAL_CALL CMouseEvent::stopPropagation()
    {
        CUIEvent::stopPropagation();
    }

    void SAL_CALL CMouseEvent::preventDefault()
    {
        CUIEvent::preventDefault();
    }

    void SAL_CALL CMouseEvent::initEvent(const OUString& eventTypeArg, sal_Bool canBubbleArg,
        sal_Bool cancelableArg)
    {
        // base initializer
        CUIEvent::initEvent(eventTypeArg, canBubbleArg, cancelableArg);
    }
}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
