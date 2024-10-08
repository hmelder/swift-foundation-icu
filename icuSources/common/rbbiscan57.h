// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//
//  Copyright (C) 2002-2016, International Business Machines Corporation and others.
//  All Rights Reserved.
//
/*
**********************************************************************
*   Legacy version of RBBIRuleScanner from ICU 57,
*   only for use by Apple RuleBasedTokenizer.
*   originally added per rdar://37249396 Add ICU 57 version of RBBI classes,
*   urbtok57 interfaces for access via RBT, and better tests
**********************************************************************
*/


#ifndef RBBISCAN57_H
#define RBBISCAN57_H

#include <_foundation_unicode/utypes.h>
#include <_foundation_unicode/uobject.h>
#include <_foundation_unicode/uniset.h>
#include <_foundation_unicode/parseerr.h>
#include "uhash.h"
#include "uvector.h"
#include <_foundation_unicode/symtable.h>// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#include "rbbi57.h"
#include "rbbinode.h"
#include "rbbirpt.h"

U_NAMESPACE_BEGIN

class   RBBIRuleBuilder57;
class   RBBISymbolTable57;


//--------------------------------------------------------------------------------
//
//  class RBBIRuleScanner57 does the lowest level, character-at-a-time
//                        scanning of break iterator rules.  
//
//                        The output of the scanner is parse trees for
//                        the rule expressions and a list of all Unicode Sets
//                        encountered.
//
//--------------------------------------------------------------------------------

class RBBIRuleScanner57 : public UMemory {
public:

    enum {
        kStackSize = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    struct RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {};
    };

    RBBIRuleScanner57(RBBIRuleBuilder57  *rb);


    virtual    ~RBBIRuleScanner57();

    void        nextChar(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       push(const RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    void        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    static UnicodeString stripRules(const UnicodeString &rules);
private:

    UBool       doParseActions(int32_t a);
    void        error(UErrorCode e);                   // error reporting convenience function.
    void        fixOpStack(RBBINode::OpPrecedence p);
                                                       //   a character.
    void        findSetFor(const UnicodeString &s, RBBINode *node, UnicodeSet *setToAdopt = NULL);

    UChar32     nextCharLL();
#ifdef RBBI_DEBUG
    void        printNodeStack(const char *title);
#endif
    RBBINode    *pushNewNode(RBBINode::NodeType  t);
    void        scanSet();


    RBBIRuleBuilder57             *fRB;              // The rule builder that we are part of.

    int32_t                       fScanIndex;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       fNextIndex;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         fQuoteMode;        // Scan is in a 'quoted region'
    int32_t                       fLineNum;          // Line number in input file.
    int32_t                       fCharNum;          // Char position within the line.
    UChar32                       fLastChar;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRuleTableEl               **fStateTable;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      fStack[kStackSize];  // State stack, holds state pushes
    int32_t                       fStackPtr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *fNodeStack[kStackSize]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        fNodeStackPtr;


    UBool                          fReverseRule;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          fLookAheadRule;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          fNoChainInRule;   // True if the current rule starts with a '^'.

    RBBISymbolTable57             *fSymbolTable;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *fSetTable;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    UnicodeSet                     fRuleSets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indicies for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        fRuleNum;         // Counts each rule as it is scanned.

    int32_t                        fOptionStart;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    UnicodeSet *gRuleSet_rule_char;
    UnicodeSet *gRuleSet_white_space;
    UnicodeSet *gRuleSet_name_char;
    UnicodeSet *gRuleSet_name_start_char;

    RBBIRuleScanner57(const RBBIRuleScanner57 &other) = delete; // forbid copying of this class
    RBBIRuleScanner57 &operator=(const RBBIRuleScanner57 &other) = delete; // forbid copying of this class
};

U_NAMESPACE_END

#endif
