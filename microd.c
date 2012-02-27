
// This implements microD

#include "microd.h"

#include <assert.h>
#include<typeinfo>

#include "rmem.h"
#include "root.h"

#include "module.h"
#include "template.h"
#include "declaration.h"
#include "statement.h"
#include "attrib.h"
#include "init.h"
#include "aggregate.h"
#include "arraytypes.h"
#include "id.h"

Dsymbols xdeferred;

OutBuffer buf1; // struct/union/enum forward declaration & includes
OutBuffer buf2; // struct/union/enum definition & var/func forward declaration
OutBuffer buf3; // var/func definition

void microd_decl1(const char *format, ...);
void microd_decl2(const char *format, ...);
void microd_decl3(const char *format, ...);

void microd_decl12(const char *format, ...);
void microd_decl23(const char *format, ...);
void microd_decl123(const char *format, ...);

char *comment1(const char *format, ...);
char *comment2(const char *format, ...);

void sinkImplements(md_fptr sink, ClassDeclaration* decl1, int prependComma);

//////////////////////////////////////////////////////////////////////////

struct MDState;

MDState *mds = NULL;

struct MDState
{
    VarDeclaration *sthis;
    MDState *prev;
    Module *m;
    Dsymbol *symbol;

    /////////////////////

    FuncDeclaration *getFunc()
    {
        MDState *bc = this;
        for (; bc->prev; bc = bc->prev);
        return (FuncDeclaration *)(bc->symbol);
    }

    /////////////////////

    MDState(Module *m, Dsymbol *s)
    {
        autopop = 1;
        push(m, s);
    }
    ~MDState()
    {
        if (autopop)
            pop();
    }
private:

    MDState *next;
    int autopop;

    MDState()
    {
        autopop = 0;
    }

    static void push(Module *m, Dsymbol *s)
    {
        MDState *x = new MDState;
        x->next = mds;
        mds = x;

        mds->prev = NULL;
        mds->m = m;
        mds->symbol = s;
        mds->sthis = NULL;
    }

    static void pop()
    {
        mds = mds->next;
    }
};

//////////////////////////////////////////////////////////////////////////

void getEthis(md_fptr sink, Loc loc, FuncDeclaration *fd);
void callfunc(md_fptr sink, int directcall, Type *tret, Expression *ec, Type *ectype,
              FuncDeclaration *fd, Type *t, Expression *ehidden, Expressions *arguments);
void escapeString(md_fptr sink, StringExp *se);

//////////////////////////////////////////////////////////////////////////

void microd_generate(Modules *modules)
{

    for (size_t i = 0; i < modules->dim; i++)
    {
        Module *m = modules->tdata()[i];
        if (global.params.verbose)
            printf("microd gen %s\n", m->toChars());

        m->toMicroD();
    }

    char *n = FileName::name((*global.params.objfiles)[0]);
    File *mdfile = new File(FileName::forceExt(n, "js"));
    mdfile->ref = 1;

    buf1.writestring(buf2.toChars());
    buf1.writestring(buf3.toChars());

    buf1.writestring("if(_Dmain != null) _Dmain();\n");

    mdfile->setbuffer(buf1.data, buf1.offset);
    mdfile->writev();
}

//////////////////////////////////////////////////////////////////////////

void Module::toMicroD()
{
    for (size_t i = 0; i < members->dim; i++)
    {
        Dsymbol *s = (*members)[i];
        s->toMicroD();
    }

    while (xdeferred.dim != 0)
    {
        Dsymbol *s = xdeferred[0];
        xdeferred.remove(0);
        s->toMicroD();
    }
}

//////////////////////////////////////////////////////////////////////////

void Dsymbol::toMicroD()
{
    printf("ignored: %s [%s] %s\n", kind(), typeid(*this).name(), toChars());
    assert(!isStructDeclaration());
}

void AttribDeclaration::toMicroD()
{
	if(suppress_js_output)
		return;
    Dsymbols *d = include(NULL, NULL);

    if (d)
    {
        for (size_t i = 0; i < d->dim; i++)
        {
            Dsymbol *s = (*d)[i];
            s->toMicroD();
        }
    }
}

void SymbolExp::toMicroD(md_fptr sink) {
	if(var)
	var->toMicroD(sink);
}

//////////////////////////////////////////////////////////////////////////

void DelegateExp::toMicroD(md_fptr sink) {
	if(!func)
		return;
	if(!func->isFuncLiteralDeclaration())
		func->toMicroD(sink);
	else
		func->toMicroD();//sink);
}

void FuncDeclaration::toMicroD(md_fptr sink) {
	if(suppress_js_output) return;

	if(isNested())
		toMicroD();
	else {
		if(linkage == LINKjs) {
			AggregateDeclaration* parent = isThis();
			char* name = NULL;
			if(parent) {
				name = parent->toChars();
				// D considers these to be a part of the class, but we want a part of the object...
				// it is important for at least the basics to work here to do feature detection of browsers
				if(strcmp(name, "JSDocument") == 0) // HACK HACK HACK HACK
					name = "document";
				else if(strcmp(name, "JSWindow") == 0) // HACK
					name = "window";
				else {
					//sink("__d_"); // to get to the class definition
					//name = parent->mangle();
					name = NULL;
				}
			}
		// if it is a member, we have to sink that too...

			if(name) {
				sink(name);
				sink(".");
				sink(toChars());
			} else {
				sink(mangle());
			}
		} else
			sink(mangle());
	}
}

void FuncDeclaration::toMicroD()
{
	if(suppress_js_output) return;
    // Find module m for this function
    Module *m = NULL;
    for (Dsymbol *p = parent; p; p = p->parent)
    {
        m = p->isModule();
        if (m)
            break;
    }
    MDState xmds(m, this);

    assert(type->ty == Tfunction);
    md_fptr sink = &microd_decl3;

    /// functions without bodies shouldn't be there in JS
    /// it is probably just a built in
    if(!fbody) {
    	return;
    }

    TypeFunction *tf = (TypeFunction *)type;
    // tf->next->toMicroD(sink);
    sink("function ");
    if(!isFuncLiteralDeclaration())
	    sink(mangle());
    sink("(");
    for (size_t i = 0; parameters && i < parameters->dim; i++)
    {
        if (i != 0)
            sink(", ");
        VarDeclaration *p = (*parameters)[i];
        p->toMicroD(sink);
    }
    if (vthis)
    {
	mds->sthis = vthis;

	if(isCtorDeclaration()) {

	} else {
            if (tf->parameters->dim)
                sink(", ");
	        vthis->toMicroD(sink);
	}
    }
    // Body
    if (fbody)
    {
        sink(") {\n");
	if(isCtorDeclaration() && vthis) {
		/* JS actually constructs the object... */
		sink("var ");
		vthis->toMicroD(sink);
		sink(" = this;\n"); // new Object();\n");
	} else if(vthis) {
		// fallback for regular javascript thingy
		sink("if(!");
		vthis->toMicroD(sink);
		sink(") var ");
		vthis->toMicroD(sink);
		sink(" = this;\n");
	}
        fbody->toMicroD(sink);
        sink("}\n");
    }
    else
        sink(");\n");
}

void ScopeDsymbol::toMicroD() {
	if(suppress_js_output) return;
	for(unsigned i = 0; i < members->dim; i++) {
		Dsymbol* s = (Dsymbol*) members->data[i];
		if(s->isFuncDeclaration()) {
			if(s->isFuncDeclaration()->parameters)
				s->toMicroD();
			else
				printf("ignoring (a template definition?) %s\n", s->toPrettyChars()); // s->toMicroD();
		}else
			printf("ignoring non function %s\n", s->toChars());
	}
}

void VarDeclaration::toMicroD()
{
	if(suppress_js_output) return;
    md_fptr sink = &microd_decl23;

    if(linkage == LINKjs) // don't put out things that are supposed to be in js
    	return;

    type->toMicroD(sink);
    sink(" ");
    sink(mangle());

    if (init)
    {
        microd_decl3(" = ");
	init->toMicroD(&microd_decl3);
    }
    sink(";\n");
}

void StructDeclaration::toMicroD()
{
	if(suppress_js_output) return;
    char *name = mangle();

    md_fptr sink = &microd_decl2;

    sink("function __d_");
    sink(name);
    sink("() {\n");

    for (size_t i = 0; i < members->dim; i++)
    {
        Dsymbol *s = (*members)[i];

        if (Declaration *vd = s->isVarDeclaration())
        {
            vd->toMicroD(sink);
            sink(";\n");
        }
        else if (FuncDeclaration *fd = s->isFuncDeclaration())
        {
            xdeferred.push(fd);
        }
        else
        {
            s->error(" %s not supported in MicroD", typeid(*this).name());
            sink("null/*__dsymbol__*/;\n");
        }
    }

    sink("}\n");
}

void sinkvirtual(md_fptr sink, ClassDeclaration* cd, FuncDeclaration* fd) {
	// adds it to the vtable
	sink("this.__d_vtbl[%d] = ", fd->vtblIndex);
	fd->toMicroD(sink);
	sink(";\n");
}

void sinkclassbase(md_fptr sink, ClassDeclaration* base) {
	if(base == NULL)
		return;
	sinkclassbase(sink, base->baseClass);
	// this will do reverse order so Object is the first one initialized
	sink("__d_");
	sink(base->mangle());
	sink(".call(this);\n");
}

void ClassDeclaration::toMicroD()
{
	if(suppress_js_output) return;
    char *name = mangle();

    md_fptr sink = &microd_decl2;

	// classes have constructors, so we don't need this kind of initializer. or do we?
    sink("function __d_");
    sink(name);
    sink("(__d_constructor) {\n"); // constructor args are passed as variadic fyi

    assert(this->baseclasses != NULL);

    if(this->baseClass == NULL) {
    	// this is Object itself, so declare the vtable
	sink("this.__d_vtbl = [];\n");
    }

    // call base classes too. this will set up the parent's initializers and the basic methods for inheritance
    sinkclassbase(sink, this->baseClass);
    

    sink("this.__d_vtbl.length = %d;\n", vtbl.dim); // preparing our own vtable to have the slots filled in later

    for (size_t i = 0; i < members->dim; i++)
    {
        Dsymbol *s = (*members)[i];

	if(s->suppress_js_output)
		continue;

        if (Declaration *vd = s->isVarDeclaration())
        {
            vd->toMicroD(sink);
            sink(";\n");
        }
        else if (FuncDeclaration *fd = s->isFuncDeclaration())
        {
		// we also want to put it in this function as properties so overriding works...
            xdeferred.push(fd);

	    if(fd->isVirtual()) {
	    	sinkvirtual(sink, this, fd);
	    }
        }
	else if(AttribDeclaration* f = s->isAttribDeclaration()) {
			    Dsymbols *d = f->include(NULL, NULL);

			    if (d)
			    {
				for (size_t i = 0; i < d->dim; i++)
				{
				    Dsymbol *s = (*d)[i];
				    if(FuncDeclaration* fd = s->isFuncDeclaration()) {
            					xdeferred.push(fd);
									
					    if(fd->isVirtual()) {
					    	sinkvirtual(sink, this, fd);
					    }
				    } else
				    s->toMicroD();
				}
			    }

	}
        else
        {
            s->error("not supported in MicroD %s", typeid(*s).name());
            //sink("__dsymbol__;\n");
        }
    }

    // add some run time type information
    sink("this.__d_classname = \"");
    sink(toPrettyChars());
    sink("\";\n");
    sink("this.__d_mangle = \"");
    sink(mangle());
    sink("\";\n");
    sink("this.__d_implements = [");
    	// it implements itself...
		sink("\"");
    		sink(mangle());
		sink("\"");
	// and whatever else it inherits from
    sinkImplements(sink, this, 1);
    sink("];\n");

    // now we have to call the constructor with the arguments given...
    sink("return __d_construct.call(this, __d_constructor, arguments);");

    sink("}\n");
}


void sinkImplements(md_fptr sink, ClassDeclaration* decl1, int prependComma) {
    if(decl1->baseclasses)
    for(unsigned a = 0; a < decl1->baseclasses->dim; a++) {
	ClassDeclaration* decl = decl1->baseclasses->tdata()[a]->base;
	if(decl) {
		if(prependComma)
			sink(", ");
		sink("\"");
		sink(decl->mangle());
		sink("\"");
		prependComma = 1;

		sinkImplements(sink, decl, prependComma);
	}
    }
}


//////////////////////////////////////////////////////////////////////////

void Declaration::toMicroD(md_fptr sink)
{
    error("Declaration %s not supported ('%s')", typeid(*this).name(), toChars());
    sink("__Declaration__");
}

void FuncExp::toMicroD(md_fptr sink) {
	if(fd)
		fd->toMicroD();
}

void VarDeclaration::toMicroD(md_fptr sink)
{
    if(!isParameter() && !isThis()) {
    	type->toMicroD(sink);
	    sink(" ");
    }

    if(isThis()) {
	sink("this.");
    }

    sink(mangle());


    if (init)
    {
        sink(" = ");
        ExpInitializer *ie = init->isExpInitializer();
        if (ie && (ie->exp->op == TOKconstruct || ie->exp->op == TOKblit))
        {
            Expression *ex = ((AssignExp *)ie->exp)->e2;
            if (ex->op == TOKint64 && type->ty == Tstruct)
                goto Ldefault;
            else
                ex->toMicroD(sink);
        }
        else if (ie->exp->op == TOKint64 && type->ty == Tstruct)
            goto Ldefault;
        else
            init->toMicroD(sink);
    }
    else if (!isParameter() && !isThis())
    {
        sink(" = ");
    Ldefault:
        type->defaultInitLiteral(loc)->toMicroD(sink);
    }

    if(!init && isThis()) {
    	sink(" = ");
	type->defaultInitLiteral(loc)->toMicroD(sink);
    }
}

void ArrayLiteralExp::toMicroD(md_fptr sink) {
	sink("[");
	if(elements)
		for(unsigned i = 0; i < elements->dim; i++) {
			if(i != 0)
				sink(", ");
			Expression* e = (Expression*) elements->data[i];
			e->toMicroD(sink);
		}
	sink("]");
}

//////////////////////////////////////////////////////////////////////////

void Type::toMicroD(md_fptr sink)
{
//    error(0, "Type '%s' not supported in MicroD", toChars());
    sink("var");
}

void TypeArray::toMicroD(md_fptr sink)
{
	sink("var");
}

void TypeBasic::toMicroD(md_fptr sink)
{
	sink("var");
	/*
    switch(ty)
    {
    case Tvoid:
    case Tint8:
    case Tuns8:
    case Tint16:
    case Tuns16:
    case Tint32:
    case Tuns32:
        sink("__d_%s", toChars());
        return;
    default:
        Type::toMicroD(sink);
    }
    */
}

void TypeStruct::toMicroD(md_fptr sink)
{
    sink("var");
}

//////////////////////////////////////////////////////////////////////////

void Parameter::toMicroD(md_fptr sink)
{
    //type->toMicroD(sink);
    //sink(" ");
    sink(ident->toChars());
    if (defaultArg)
    {
        sink(" = ");
        defaultArg->toMicroD(sink);
    }
}

//////////////////////////////////////////////////////////////////////////

void Expression::toMicroD(md_fptr sink)
{
    error("Expression %s not supported in MicroD ('%s')", typeid(*this).name(), toChars());
    //sink(toChars());
    sink("__expression__");
}

void argsToMicroD(md_fptr sink, Expressions* arguments)
{
    if (arguments)
    {
        for (size_t i = 0; i < arguments->dim; i++)
        {   Expression *arg = arguments->tdata()[i];

            if (arg)
            {   if (i)
                    sink(",");
		arg->toMicroD(sink);
            }
        }
    }
}


void NewExp::toMicroD(md_fptr sink) {
    sink("new ");
    /* // js has no placement new or anything
    if (newargs && newargs->dim)
    {
        sink("(");
        argsToMicroD(sink, newargs);
        sink(")");
    }
    */
    //sink(newtype->toChars()); // this isn't toMicroD because we want the type name, not var, to construct

	if(!member) {
		// FIXME: what if it is a custom D type without a constructor?
		Dsymbol* sym = type->toDsymbol(NULL);
		if(sym != NULL)
			sink(sym->toChars());
//		sink(type->mangle());
		sink("()");
		return;
	}

    ClassDeclaration* classOf = member->isMember2()->isClassDeclaration();
    assert(classOf);

    sink("__d_");
    sink(classOf->mangle()); // the class we want
    sink("(");
    if(member)
	    sink(member->mangle()); // constructor
    else
	    sink("null");

    if (arguments && arguments->dim)
    {
        sink(",");
        argsToMicroD(sink, arguments); // constructor arguments
    }
    sink(")");
}

void IndexExp::toMicroD(md_fptr sink) {
	e1->toMicroD(sink);
	sink("[");
	e2->toMicroD(sink);
	sink("]");
}

void SliceExp::toMicroD(md_fptr sink) {
	sink("(");
	e1->toMicroD(sink);
	sink(")");
	sink(".slice(");

	if(upr) {
		upr->toMicroD(sink);
	} else {
		sink("0");
	}

	if(lwr) {
		sink(",");
		lwr->toMicroD(sink);
	} else {
		if(lengthVar) {
			sink(",");
			lengthVar->toMicroD(sink);
		}
	}

	sink(")");
}

void IntegerExp::toMicroD(md_fptr sink)
{
    dinteger_t v = toInteger();
    sink("%d", v);
}

void DeclarationExp::toMicroD(md_fptr sink)
{
    Declaration *d = declaration->isDeclaration();
    assert(d);
    d->toMicroD(sink);
}

void BreakStatement::toMicroD(md_fptr sink) {
	if(ident) {
		error("labeled break not supported");
	} else
		sink("break;");
}
void ContinueStatement::toMicroD(md_fptr sink) {
	if(ident) {
		error("labeled continue not supported");
	} else
		sink("continue;");
}

void ThrowStatement::toMicroD(md_fptr sink) {
	sink("throw ");
	exp->toMicroD(sink);
	sink(";");
}

void TryCatchStatement::toMicroD(md_fptr sink) {
	sink("try {\n");
	if(body)
		body->toMicroD(sink);
	sink("}\n");

	static int catchCount = 0;

	if(catches) {
		sink("catch (__d_exception_%d) {\n", ++catchCount);
		for(unsigned i = 0; i < catches->dim; i++) {
			Catch* c = (Catch*) catches->data[i];
			if(c) {
				Dsymbol* t1 = c->type->toDsymbol(NULL);
				assert(t1);
				ClassDeclaration* t = t1->isClassDeclaration();
				assert(t);

				sink("var %s = __d_dynamic_cast(__d_exception_%d, \"%s\");\n", c->var->mangle(), catchCount, t->mangle());
				sink("if(%s) {\n", c->var->mangle());
				c->handler->toMicroD(sink);
				sink("}");
			}
		}
		sink("}\n");
	}
}

void UnaExp::toMicroD(md_fptr sink) {
	switch(op) {
		case TOKneg:
		case TOKplusplus:
		case TOKminusminus:
		case TOKnot:
			sink(Token::toChars(op));
			sink("(");
		        e1->toMicroD(sink);
			sink(")");
		break;
		default:
			Expression::toMicroD(sink);
		break;
	}
}

void PostExp::toMicroD(md_fptr sink) {
	switch(op) {
		case TOKplusplus:
		case TOKminusminus:
			sink("(");
		        e1->toMicroD(sink);
			sink(")");
			sink(Token::toChars(op));
		break;
		default:
			Expression::toMicroD(sink);
		break;
	}
}

void ArrayLengthExp::toMicroD(md_fptr sink) {
	sink("(");
	e1->toMicroD(sink);
	sink(".length");
	sink(")");
}

void IdentityExp::toMicroD(md_fptr sink) {
	sink("(");
	e1->toMicroD(sink);
	if(op == TOKidentity)
		sink("===");
	else // must be !is
		sink("!==");
	e2->toMicroD(sink);
	sink(")");
}

void BinExp::toMicroD(md_fptr sink)
{
    switch (op)
    {
    case TOKlt:
    case TOKle:
    case TOKgt:
    case TOKge:
    case TOKequal:
    case TOKnotequal:

    case TOKadd:
    case TOKmin:
    case TOKmul:
    case TOKdiv:
    case TOKand:
    case TOKor:
    case TOKxor:

    case TOKaddass:
    case TOKminass:
    case TOKmulass:
    case TOKdivass:
    case TOKandass:
    case TOKorass:
    case TOKxorass:

    case TOKassign:

        sink("(");
        e1->toMicroD(sink);
        sink(" %s ", Token::toChars(op));
        e2->toMicroD(sink);
        sink(")");
        break;
    case TOKcatass:
        e1->toMicroD(sink);
        sink(" = (");
        e1->toMicroD(sink);
        sink(".concat(");
        e2->toMicroD(sink);
        sink("))");
	break;

    break;
    case TOKcat:
        sink("(");
        e1->toMicroD(sink);
        sink(".concat(");
        e2->toMicroD(sink);
        sink("))");
	break;
    default:
        Expression::toMicroD(sink);
        break;
    }
}

void CallExp::toMicroD(md_fptr sink)
{
    Type *t1 = e1->type->toBasetype();
    Type *ectype = t1;
    Expression *ec;
    FuncDeclaration *fd;
    int directcall = 0;
    Expression *ehidden = NULL;

    if (e1->op == TOKdotvar && t1->ty != Tdelegate)
    {
        DotVarExp *dve = (DotVarExp *)e1;

        fd = dve->var->isFuncDeclaration();
        Expression *ex = dve->e1;
        while (1)
        {
            switch (ex->op)
            {
                case TOKsuper:          // super.member() calls directly
                case TOKdottype:        // type.member() calls directly
                    directcall = 1;
                    break;

                case TOKcast:
                    ex = ((CastExp *)ex)->e1;
                    continue;

                default:
                    //ex->dump(0);
                    break;
            }
            break;
        }
        ec = dve->e1;
        ectype = dve->e1->type->toBasetype();
    }
    else if (e1->op == TOKvar)
    {
        fd = ((VarExp *)e1)->var->isFuncDeclaration();
        ec = e1;
    }
    else
    {
        ec = e1;
    }

    if(fd->linkage == LINKjs)
    	directcall = 1;

    callfunc(sink, directcall, type, ec, ectype, fd, t1, ehidden, arguments);
}

void DotVarExp::toMicroD(md_fptr sink)
{
    sink("(");
    e1->toMicroD(sink);
    sink(").");

    if(var->linkage == LINKjs) {
    	char* name = var->toChars();
	int len = strlen(name);
	if(len > 5 && name[0] == '_' && name[1] == '_' && name[2] == 'j' && name[3] == 's' && name[4] == '_') {
		name += 5;
	}
        sink(name);
    } else
        sink(var->mangle());
}

void VarExp::toMicroD(md_fptr sink)
{
	char* name = var->toChars();
	int len = strlen(name);
	if(len > 5 && name[0] == '_' && name[1] == '_' && name[2] == 'j' && name[3] == 's' && name[4] == '_') {
		// these correspond directly to javascript keywords in some way
		if(strcmp(name, "__js_array_literal") == 0)
			sink("[]");
		else if(strcmp(name, "__js_object_literal") == 0)
			sink("{}");
		else
			sink(name + 5); // probably a keyword or something
	} else
		sink(var->mangle());
}

void CastExp::toMicroD(md_fptr sink)
{
	// there's no such thing as a cast in JS
	// FIXME: but what about dynamic class casts?

	assert(to);

	ClassDeclaration* classOf = to->isClassHandle();

	if(classOf)
		sink("__d_dynamic_cast");

    sink("(");
    e1->toMicroD(sink);
	if(classOf) {
		sink(", \"");
		sink(classOf->mangle());
		sink("\"");
	}
    sink(")");
}

void AssertExp::toMicroD(md_fptr sink)
{
    e1->toMicroD(sink);
    sink(" || ");
    if (msg)
    {
        sink("__d_assert_msg(");
        msg->toMicroD(sink);
        sink(", ");
    }
    else
        sink("__d_assert(");

    escapeString(sink, new StringExp(0, (char*)loc.filename, strlen(loc.filename)));
    sink(", %d)", loc.linnum);
}

void StringExp::toMicroD(md_fptr sink)
{
    Type *tb = type->toBasetype();
    if (!tb->nextOf() || tb->nextOf()->ty != Tchar)
    {
        error("only utf-8 strings are supported in MicroD, not %s", toChars());
        sink("__StringExp__");
        return;
    }

    if (tb->ty == Tpointer)
    {
        escapeString(sink, this);
    }
    else if (tb->ty == Tarray)
    {
        escapeString(sink, this);
    }
    else
    {
        error("only char* strings are supported in MicroD, not %s", toChars());
        sink("__StringExp__");
        return;
    }
}

void NullExp::toMicroD(md_fptr sink)
{
    if(type == NULL) {
	    sink("null");
	    return;
    }

    if(type->isString())
    	    sink("\"\"");
    else if(type->ty == Tsarray || type->ty == Tarray)
    	    sink("[]");
    else if(type->ty == Taarray)
    	    sink("{}");
    else
    	sink("null");

}

void BoolExp::toMicroD(md_fptr sink) { // FIXME: i'm not really sure what this is
    e1->toMicroD(sink);
}

void AddrExp::toMicroD(md_fptr sink)
{
    e1->toMicroD(sink);
}

void PtrExp::toMicroD(md_fptr sink) // FIXME: should this be here?
{
    e1->toMicroD(sink);
}

void ThisExp::toMicroD(md_fptr sink)
{
    FuncDeclaration *fd;
    assert(mds->sthis);

    if (var)
    {
        assert(var->parent);
        fd = var->toParent2()->isFuncDeclaration();
        assert(fd);
        getEthis(sink, loc, fd);
    }
    else
        sink(mds->sthis->mangle());
}

void StructLiteralExp::toMicroD(md_fptr sink)
{
    sink("new __d_");
    sink(this->sd->mangle());
    sink("(");
    for (size_t i = 0; i < elements->dim; i++)
    {
        Expression *e = (*elements)[i];
        if (i)
            sink(", ");
        e->toMicroD(sink);
    }
    sink(")");
}

//////////////////////////////////////////////////////////////////////////

void Initializer::toMicroD(md_fptr sink)
{
    error("This type of initializer, %s, not supported in MicroD ('%s')", typeid(*this).name(), toChars());
    sink("__init__");
}

void ExpInitializer::toMicroD(md_fptr sink)
{
    exp->toMicroD(sink);
}

//////////////////////////////////////////////////////////////////////////

void Statement::toMicroD(md_fptr sink)
{
    error("Statement %s not supported in MicroD ('%s')", typeid(*this).name(), toChars());
    sink("__statement__;\n");
}

void SwitchStatement::toMicroD(md_fptr sink) {
	sink("switch(");
	condition->toMicroD(sink);
	sink(") {\n");
	body->toMicroD(sink);
	sink("}");
}

void CaseStatement::toMicroD(md_fptr sink) {
	sink("case ");
	exp->toMicroD(sink);
	sink(":\n");
	statement->toMicroD(sink);
}

void DefaultStatement::toMicroD(md_fptr sink) {
	sink("default:\n");
	statement->toMicroD(sink);
}

void CompoundStatement::toMicroD(md_fptr sink)
{
	if(statements)
    for (size_t i = 0; i < statements->dim; i++)
    {
        Statement *s = (*statements)[i];
	if(s)
        s->toMicroD(sink);
    }
//	sink("/* end compound */");
}

void CompoundDeclarationStatement::toMicroD(md_fptr sink)
{
    int nwritten = 0;
    for (size_t i = 0; i < statements->dim; i++)
    {
        Statement *s = (*statements)[i];
        ExpStatement *es = s->isExpStatement();
        assert(es && es->exp->op == TOKdeclaration);
        DeclarationExp *de = (DeclarationExp *)es->exp;
        Declaration *d = de->declaration->isDeclaration();
        assert(d);
        VarDeclaration *v = d->isVarDeclaration();
        if (v)
        {
            if (nwritten)
                sink(",");
            // write storage classes
            if (v->type && !nwritten)
                v->type->toMicroD(sink);
            sink(" ");
            sink(v->mangle());
            if (v->init)
            {
                sink(" = ");
                ExpInitializer *ie = v->init->isExpInitializer();
                if (ie && (ie->exp->op == TOKconstruct || ie->exp->op == TOKblit))
                    ((AssignExp *)ie->exp)->e2->toMicroD(sink);
                else
                    v->init->toMicroD(sink);
            }
        }
        else
            d->toMicroD(sink);
        nwritten++;
    }
    sink(";\n");
}

void ExpStatement::toMicroD(md_fptr sink)
{
    exp->toMicroD(sink);
    sink(";\n");
}

void IfStatement::toMicroD(md_fptr sink)
{
	sink("if (");
	condition->toMicroD(sink);
	sink(") {");
	ifbody->toMicroD(sink);
	sink("}");
	if(elsebody) {
		sink("else {");
		elsebody->toMicroD(sink);
		sink("}");
	}
}

void ConditionalStatement::toMicroD(md_fptr sink)
{
	// FIXME
}

void CondExp::toMicroD(md_fptr sink) {
	sink("(");
	sink("(");
	econd->toMicroD(sink);
	sink(")");
	sink("?(");
	e1->toMicroD(sink);
	sink("):(");
	e2->toMicroD(sink);
	sink("))");
}

void ScopeStatement::toMicroD(md_fptr sink) {
	if(statement)
	statement->toMicroD(sink);
}

void ForStatement::toMicroD(md_fptr sink)
{
    sink("for (");
    init->toMicroD(sink);
    condition->toMicroD(sink);
    sink("; ");
    increment->toMicroD(sink);
    sink(") {");
    body->toMicroD(sink);
    sink("}\n");
}

void WhileStatement::toMicroD(md_fptr sink) {
	sink("while (");
	condition->toMicroD(sink);
	sink(") {\n");
	body->toMicroD(sink);
	sink("}\n");
}

void DoStatement::toMicroD(md_fptr sink) {
	sink("do {\n");
	body->toMicroD(sink);
	sink("} while(");
	condition->toMicroD(sink);
	sink(");\n");
}

void ForeachStatement::toMicroD(md_fptr sink) {
	// this space intentionally left blank (it will do a for i think)
}

void ReturnStatement::toMicroD(md_fptr sink)
{
    sink("return ");
    exp->toMicroD(sink);
    sink(";\n");
}

//////////////////////////////////////////////////////////////////////////

void microd_decl1(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    buf1.vprintf(format,ap);
    va_end(ap);
}

void microd_decl2(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    buf2.vprintf(format,ap);
    va_end(ap);
}

void microd_decl3(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    buf3.vprintf(format,ap);
    va_end(ap);
}

void microd_decl12(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    buf1.vprintf(format,ap);
    buf2.vprintf(format,ap);
    va_end(ap);
}

void microd_decl23(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    buf2.vprintf(format,ap);
    buf3.vprintf(format,ap);
    va_end(ap);
}

void microd_decl123(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    buf1.vprintf(format,ap);
    buf2.vprintf(format,ap);
    buf3.vprintf(format,ap);
    va_end(ap);
}


char *comment1(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    OutBuffer buf;
    buf.writestring("/***********************************************************\n * \n * ");
    buf.vprintf(format, ap);
    buf.writestring("\n * \n */\n\n");
    va_end(ap);
    return buf.extractData();
}
char *comment2(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    OutBuffer buf;
    buf.writestring("/***********************************************************\n * ");
    buf.vprintf(format, ap);
    buf.writestring("\n */\n\n");
    va_end(ap);
    return buf.extractData();
}

void getEthis(md_fptr sink, Loc loc, FuncDeclaration *fd)
{
    FuncDeclaration *thisfd = mds->getFunc();
    Dsymbol *fdparent = fd->toParent2();

    if (fd->ident == Id::require || fd->ident == Id::ensure)
        assert(0);

    if (fdparent == thisfd)
    {
        //if (mds->sclosure)

        if (mds->sthis)
        {
            sink(mds->sthis->mangle());
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        if (!mds->sthis)
        {
            fd->error(loc, "is a nested function and cannot be accessed from %s", mds->getFunc()->toPrettyChars());
            sink("__ethis__");
            return;
        }
        else
        {
            VarDeclaration *ethis = mds->sthis;
            Dsymbol *s = thisfd;
            while (fd != s)
            {
                thisfd = s->isFuncDeclaration();
                if (thisfd)
                {
                    if (fdparent == s->toParent2())
                        break;
                    if (thisfd->isNested())
                    {
                        FuncDeclaration *p = s->toParent2()->isFuncDeclaration();
                        if (!p || p->hasNestedFrameRefs())
                            sink("*");
                    }
                    else if (thisfd->vthis)
                    {
                    }
                    else
                    {   // Error should have been caught by front end
                        assert(0);
                    }
                }
                else
                {
                    assert(0);
                }
                s = s->toParent2();
                assert(s);
            }
            sink(ethis->mangle());
        }
    }
}

void callfunc(md_fptr sink, int directcall, Type *tret, Expression *ec, Type *ectype,
              FuncDeclaration *fd, Type *t, Expression *ehidden, Expressions *arguments)
{

    int useVtbl = 0;
    int useDotSyntax = 0;
    if(fd->linkage == LINKjs)
    	useDotSyntax = 1;

    t = t->toBasetype();
    //TypeFunction *tf;
    Expression *ethis = NULL;

	/*
    if (t->ty == Tdelegate)
    {
        ec->error("delegate calls are not supported in MicroD");
        sink("__callfunc__");
        return;
    }
    else
    {
        assert(t->ty == Tfunction);
        tf = (TypeFunction *)t;
    }
    	*/

    if (fd && fd->isMember2())
    {
        AggregateDeclaration *ad = fd->isThis();

        if (ad)
        {
            ethis = ec;
            if (ad->isStructDeclaration() && ectype->toBasetype()->ty != Tpointer)
                ethis = ethis->addressOf(NULL);
        }
        else
        {
            assert(0);
        }

        if (!fd->isVirtual() ||
            directcall ||
            fd->isFinal())
        {
            ec = new VarExp(0, fd);
        }
        else
        {
		// virtual function
		useDotSyntax = 1;
		useVtbl = 1;
        }
    }

    //if (tf->isref)
     //   sink("*");

   if(useDotSyntax && ethis && !fd->isCtorDeclaration()) {
        ethis->toMicroD(sink);
        sink(".");
	if(useVtbl)
		sink("__d_vtbl[%d]", fd->vtblIndex);
	else
	sink(ec->toChars()); // original name so JS can see it
   } else {
    // mangled name
        ec->toMicroD(sink);
   }

   int putComma = 0;

   if(fd->isCtorDeclaration() && ethis) {
	sink(".call(");
	putComma = 1;
	ethis->toMicroD(sink);
   } else
       sink("(");

    if (arguments)
    {
        int j = 0;//(tf->linkage == LINKd && tf->varargs == 1);

        for (size_t i = 0; i < arguments->dim; i++)
        {
            if (i != 0 || putComma)
                sink(", ");

            Expression *arg = (*arguments)[i];
            //size_t nparams = Parameter::dim(tf->parameters);
            //if (i - j < nparams && i >= j)
            //{
                //Parameter *p = Parameter::getNth(tf->parameters, i - j);

        //        if (p->storageClass & (STCout | STCref))
          //          sink("&");
            //}
            arg->toMicroD(sink);
        }
    }

    if (!useDotSyntax && ethis)
    {
        if (arguments && arguments->dim)
            sink(", ");
        ethis->toMicroD(sink);
    }
    sink(")");
}

void escapeString(md_fptr sink, StringExp *se)
{
    sink("\"");
    for (size_t i = 0; i < se->len; i++)
    {
        unsigned c = se->charAt(i);

        switch (c)
        {
            case '"':
            case '\\':
                sink("\\");
            default:
                if (c <= 0xFF)
                {   if (c <= 0x7F && isprint(c))
                        sink("%c", c);
                    else
                        sink("\\x%02x", c);
                }
                else if (c <= 0xFFFF)
                    sink("\\x%02x\\x%02x", c & 0xFF, c >> 8);
                else
                    sink("\\x%02x\\x%02x\\x%02x\\x%02x",
                        c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF, c >> 24);
                break;
        }
    }
    sink("\"");
}
