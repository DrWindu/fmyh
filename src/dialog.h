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


#ifndef _LAIR_DEMO_TEMPLATE_DIALOG_H
#define _LAIR_DEMO_TEMPLATE_DIALOG_H


using namespace lair;


/* A dialog is a graph of conditional lines with effects.
 *
 * A condition is a propositional logic formula based on (boolean) flags
 * and (integer) value comparisons, and may be flagged as a player choice.
 *
 * Lines is a sequence of utterances tied to characters.
 *
 * An effect is list of flags being set or unset, or +/-/= changes in values.
 *
 * Syntax-wise, each dialog occupies an .ldl file as map of nodes, keyed by id.
 * Each node is itself a map using the keys: conditions, lines, effects, next.
 * Empty entries may be ommited.
 *
 * Conditions is a possibly empty string, defined by :
 * CONDITIONS: [ "@Pick" ] [ LOGIC ]
 * LOGIC: '(' LOGIC ')'    | 'NOT' LOGIC
 *      | LOGIC 'OR' LOGIC | LOGIC 'AND' LOGIC
 *      | '>'STATUS        | '<'STATUS         | '='STATUS
 *      | FLAG             | '!'FLAG
 * STATUS: VAR'['VAL']'
 * FLAG is a flag name [A-Za-z_].
 * VAR is an in-game variable name [A-Za-z_].
 * VAL is an integer value.
 *
 * Lines is either a list of or just one pair of strings : [character, text].
 * In the former case, each line is read sequentially.
 *
 * Effects is a list of strings, each defined by :
 * EFFECT: '+'CHANGE | '-'CHANGE | '='CHANGE
 *       | FLAG      | '!'FLAG
 * CHANGE is the same as a STATUS, FLAG is like above.
 *
 * Next is a list of ids of possible followup nodes, sorted by priority.
 */

class Dialog : {
public:
	Dialog();
	virtual ~Dialog();

protected:
};


#endif
