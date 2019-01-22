/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_COMPILERPLUGINS_CLANG_FUNCTIONADDRESS_HXX
#define INCLUDED_COMPILERPLUGINS_CLANG_FUNCTIONADDRESS_HXX

#include <clang/AST/RecursiveASTVisitor.h>
#include <unordered_set>
#include "plugin.hxx"

/**
 * Common code for checking if the address of a function was taken.
 */
namespace loplugin {

template<typename Derived>
class FunctionAddress : public RecursiveASTVisitor<Derived>, public loplugin::Plugin
{
public:
    explicit FunctionAddress( const InstantiationData& data ) : loplugin::Plugin(data) {}

    bool TraverseCallExpr(CallExpr * expr) {
        auto const saved = callee_;
        callee_ = expr->getCallee();
        auto const ret = RecursiveASTVisitor<Derived>::TraverseCallExpr(expr);
        callee_ = saved;
        return ret;
    }

    bool TraverseCXXOperatorCallExpr(CXXOperatorCallExpr * expr) {
        auto const saved = callee_;
        callee_ = expr->getCallee();
        auto const ret = RecursiveASTVisitor<Derived>::TraverseCXXOperatorCallExpr(expr);
        callee_ = saved;
        return ret;
    }

    bool TraverseCXXMemberCallExpr(CXXMemberCallExpr * expr) {
        auto const saved = callee_;
        callee_ = expr->getCallee();
        auto const ret = RecursiveASTVisitor<Derived>::TraverseCXXMemberCallExpr(expr);
        callee_ = saved;
        return ret;
    }

    bool TraverseCUDAKernelCallExpr(CUDAKernelCallExpr * expr) {
        auto const saved = callee_;
        callee_ = expr->getCallee();
        auto const ret = RecursiveASTVisitor<Derived>::TraverseCUDAKernelCallExpr(expr);
        callee_ = saved;
        return ret;
    }

    bool TraverseUserDefinedLiteral(UserDefinedLiteral * expr) {
        auto const saved = callee_;
        callee_ = expr->getCallee();
        auto const ret = RecursiveASTVisitor<Derived>::TraverseUserDefinedLiteral(expr);
        callee_ = saved;
        return ret;
    }

    bool VisitImplicitCastExpr(ImplicitCastExpr const * expr) {
        if (expr == callee_) {
            return true;
        }
        if (ignoreLocation(expr)) {
            return true;
        }
        if (expr->getCastKind() != CK_FunctionToPointerDecay) {
            return true;
        }
        auto const dre = dyn_cast<DeclRefExpr>(
            expr->getSubExpr()->IgnoreParens());
        if (dre == nullptr) {
            return true;
        }
        auto const fd = dyn_cast<FunctionDecl>(dre->getDecl());
        if (fd == nullptr) {
            return true;
        }
        ignoredFunctions_.insert(fd->getCanonicalDecl());
        return true;
    }

    bool VisitUnaryAddrOf(UnaryOperator const * expr) {
        if (ignoreLocation(expr)) {
            return true;
        }
        auto const dre = dyn_cast<DeclRefExpr>(
            expr->getSubExpr()->IgnoreParenImpCasts());
        if (dre == nullptr) {
            return true;
        }
        auto const fd = dyn_cast<FunctionDecl>(dre->getDecl());
        if (fd == nullptr) {
            return true;
        }
        ignoredFunctions_.insert(fd->getCanonicalDecl());
        return true;
    }

protected:
    std::unordered_set<FunctionDecl const *> const & getFunctionsWithAddressTaken() { return ignoredFunctions_; }

private:
    std::unordered_set<FunctionDecl const *> ignoredFunctions_;
    Expr const * callee_ = nullptr;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
