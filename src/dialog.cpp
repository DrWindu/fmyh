/*
 *  Copyright (C) 2015, 2016 Simon Boyé
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <lair/core/ldl.h>

#include "dialog.h"
#include "logic_language.tab.hpp"

#ifdef __APPLE__
#include "fmemopen.h"
#else
#include <cstdio>
#endif

extern FILE* yyin;

using namespace lair;

void parseLogic (const String mess, DNode& n)
{
	yyin = fmemopen((char*) mess.c_str(), mess.size(), "r");
	yyparse();
}

void readDLine (LdlParser& p, std::vector<DLine>& lines)
{
	String dood, text;
	bool ok = true;
	ok = ok && ldlRead(p, dood);
	ok = ok && ldlRead(p, text);
	if(ok)
		lines.push_back({dood, text});
}

void listDLines (LdlParser& p, DNode& n)
{
	if(p.valueType() != LdlParser::TYPE_LIST) {
		p.error("Expected VarList, got ", p.valueTypeName());
		p.skip();
		return;
	}

	p.enter();
	if (p.valueType() == LdlParser::TYPE_STRING)
		readDLine(p, n.lines);
	else if(p.valueType() == LdlParser::TYPE_LIST) {
		while (p.valueType() != LdlParser::TYPE_END)
		{
			p.enter();
			readDLine(p, n.lines);
			p.leave();
		}
	}
	else {
		p.error("Expected VarList or String, got ", p.valueTypeName());
		p.skip();
	}
	p.leave();
}

DEffect parseDEffect (String s)
{
	int p = 0;
	DEffect e;
	switch (s[0])
	{
		case '+':
			e.t = V_INCR;
			break;
		case '-':
			e.t = V_DECR;
			break;
		case '=':
			e.t = V_SET;
			break;

		case '!':
			e.t = UNFLAG;
			break;
		default:
			e.t = FLAG;
			p--;
	}

	s = s.substr(++p);
	p = s.find('[');
	
	e.name = s.substr(0,p);
	if (p != std::string::npos)
		e.val = atoi(s.substr(p+1, s.size()-p-2).data()); // Ugh.

	return e;
}

void listDEffects (LdlParser& p, DNode& n)
{
	if (p.valueType() == LdlParser::TYPE_STRING)
	{
		parseDEffect(p.getString());
		p.next();
		return;
	}

	p.enter();
	while (p.valueType() != LdlParser::TYPE_END)
	{
		parseDEffect(p.getString());
		p.next();
	}
	p.leave();
}

void stackNexts (LdlParser& p, std::vector<String> next)
{
	//TODO: This.
	p.skip();
}

Dialog::Dialog(const Path& filename)
{
	Path::IStream in(filename.native().c_str());
	lairAssert(in.good());
	ErrorList el;
	LdlParser p(&in, filename.utf8String(), &el, LdlParser::CTX_MAP);

	std::vector<DNode*> nodes;
	std::vector<std::vector<String>> nexts;

	if(p.valueType() != LdlParser::TYPE_MAP) {
		p.error("Expected VarMap, got ", p.valueTypeName());
		p.skip();
		return;
	}

	p.enter();
	while (p.valueType() != LdlParser::TYPE_END)
	{
		if(p.valueType() != LdlParser::TYPE_MAP) {
			p.error("Expected VarMap, got ", p.valueTypeName());
			p.skip();
			continue;
		}
		
		DNode* n = new DNode;
		nodes.push_back(n);
		nexts.push_back(std::vector<String>());

		n->id = p.getKey();
		p.enter();
		while (p.valueType() != LdlParser::TYPE_END)
		{
			String nkey = p.getKey();
			if (nkey == "conditions") {
				if(p.valueType() != LdlParser::TYPE_STRING) {
					p.error("Expected String, got ", p.valueTypeName());
					p.skip();
				}
				else {
					String cond;
					ldlRead(p, cond);
					parseLogic(cond, *n);
				}
			}
			else if (nkey == "lines")
				listDLines(p, *n);
			else if (nkey == "effects")
				listDEffects(p, *n);
			else if (nkey == "next")
				stackNexts(p, nexts.back());
			else
				p.skip();
		}
		p.leave();		
	}

	//TODO: Build the actual graph using nexts.
	el.log(dbgLogger);

	_start = nodes[0];
}

Dialog::~Dialog()
{
}

