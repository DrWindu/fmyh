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

using namespace lair;

void parseLogic (const String mess, DNode& n)
{
	// Fuck this. I quit.
}

void readDLine (LdlParser& p, std::vector<DLine>& lines)
{
	String dood, text;
	ldlRead(p, dood);
	ldlRead(p, text);
	lines.push_back({dood, text});
}

void listDLines (LdlParser& p, DNode& n)
{
	p.enter();
	if (p.valueType() == LdlParser::TYPE_STRING)
		readDLine(p, n.lines);
	else while (p.valueType() != LdlParser::TYPE_END)
	{
		p.enter();
		readDLine(p, n.lines);
		p.leave();
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
}

Dialog::Dialog(const Path& filename)
{
	Path::IStream in(filename.native().c_str());
	ErrorList el;
	LdlParser p(&in, filename.utf8String(), &el, LdlParser::CTX_MAP);

	std::vector<DNode*> nodes;
	std::vector<std::vector<String>> nexts;

	while (p.valueType() != LdlParser::TYPE_END)
	{
		DNode* n = new DNode;
		nodes.push_back(n);
		nexts.push_back(std::vector<String>());

		n->id = p.getKey();
		p.enter();
		while (p.valueType() != LdlParser::TYPE_END)
		{
			String nkey = p.getKey();
			if (nkey == "conditions")
				parseLogic(p.getString(), *n);
			else if (nkey == "lines")
				listDLines(p, *n);
			else if (nkey == "effects")
				listDEffects(p, *n);
			else if (nkey == "next")
				stackNexts(p, nexts.back());
			p.next();
		}
		p.leave();		
	}

	//TODO: Build the actual graph using nexts.

	_start = nodes[0];
}

Dialog::~Dialog()
{
}

