/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

/*
To help you in your selection of option switch names, here's section 10.5
of The Art of Unix Programming, Copyright 2003 Eric S. Raymond.

The book as a whole is available online at <http://www.faqs.org/docs/artu/>.

The Art of Unix Programming
Chapter 10. Configuration

10.5 Command-Line Options

Unix tradition encourages the use of command-line switches to control programs,
so that options can be specified from scripts. This is especially important for
programs that function as pipes or filters. Three conventions for how to
distinguish command-line options from ordinary arguments exist; the original
Unix style, the GNU style, and the X toolkit style.

In the original Unix tradition, command-line options are single letters preceded
by a single hyphen. Mode-flag options that do not take following arguments can
be ganged together; thus, if -a and -b are mode options, -ab or -ba is also
correct and enables both. The argument to an option, if any, follows it
(optionally separated by whitespace). In this style, lowercase options are
preferred to uppercase. When you use uppercase options, it's good form for them
to be special variants of the lowercase option.

The original Unix style evolved on slow ASR-33 teletypes that made terseness a
virtue; thus the single-letter options. Holding down the shift key required
actual effort; thus the preference for lower case, and the use of "-" (rather
than the perhaps more logical "+") to enable options.

The GNU style uses option keywords (rather than keyword letters) preceded by two
hyphens. It evolved years later when some of the rather elaborate GNU utilities
began to run out of single-letter option keys (this constituted a patch for the
symptom, not a cure for the underlying disease). It remains popular because GNU
options are easier to read than the alphabet soup of older styles. GNU-style
options cannot be ganged together without separating whitespace. An option
argument (if any) can be separated by either whitespace or a single "=" (equal
sign) character.

The GNU double-hyphen option leader was chosen so that traditional single-letter
options and GNU-style keyword options could be unambiguously mixed on the same
command line. Thus, if your initial design has few and simple options, you can
use the Unix style without worrying about causing an incompatible 'flag day' if
you need to switch to GNU style later on. On the other hand, if you are using
the GNU style, it is good practice to support single-letter equivalents for at
least the most common options.

The X toolkit style, confusingly, uses a single hyphen and keyword options. It
is interpreted by X toolkits that filter out and process certain options (such
as -geometry and -display) before handing the filtered command line to the
application logic for interpretation. The X toolkit style is not properly
compatible with either the classic Unix or GNU styles, and should not be used in
new programs unless the value of being compatible with older X conventions seems
very high.

Many tools accept a bare hyphen, not associated with any option letter, as a
pseudo-filename directing the application to read from standard input. It is
also conventional to recognize a double hyphen as a signal to stop option
interpretation and treat all following arguments literally.

Most Unix programming languages offer libraries that will parse a command line
for you in either classic-Unix or GNU style (interpreting the double-hyphen
convention as well).

The -a to -z of Command-Line Options

Over time, frequently-used options in well-known Unix programs have established
a loose sort of semantic standard for what various flags might be expected to
mean. The following is a list of options and meanings that should prove usefully
unsurprising to an experienced Unix user:

-a
All (without argument). If there is a GNU-style --all option, for -a to be
anything but a synonym for it would be quite surprising. Examples: fuser(1),
fetchmail(1).

Append, as in tar(1). This is often paired with -d for delete.

-b
Buffer or block size (with argument). Set a critical buffer size, or (in a
program having to do with archiving or managing storage media) set a block size.
Examples: du(1), df(1), tar(1).

Batch. If the program is naturally interactive, -b may be used to suppress
prompts or set other options appropriate to accepting input from a file rather
than a human operator. Example: flex(1).

 -c
Command (with argument). If the program is an interpreter that normally takes
commands from standard input, it is expected that the option of a -c argument
will be passed to it as a single line of input. This convention is particularly
strong for shells and shell-like interpreters. Examples: sh(1), ash(1), bsh(1),
ksh(1), python(1). Compare -e below.

Check (without argument). Check the correctness of the file argument(s) to the
command, but don't actually perform normal processing. Frequently used as a
syntax-check option by programs that do interpretation of command files.
Examples: getty(1), perl(1).

-d
Debug (with or without argument). Set the level of debugging messages. This one
is very common.

Occasionally -d has the sense of 'delete' or 'directory'.

-D
Define (with argument). Set the value of some symbol in an interpreter,
compiler, or (especially) macro-processor-like application. The model is the use
of -D by the C compiler's macro preprocessor. This is a strong association for
most Unix programmers; don't try to fight it.

-e
Execute (with argument). Programs that are wrappers, or that can be used as
wrappers, often allow -e to set the program they hand off control to. Examples:
xterm(1), perl(1).

Edit. A program that can open a resource in either a read-only or editable mode
may allow -e to specify opening in the editable mode. Examples: crontab(1), and
the get(1) utility of the SCCS version-control system.

Occasionally -e has the sense of 'exclude' or 'expression'.

-f
File (with argument). Very often used with an argument to specify an input (or,
less frequently, output) file for programs that need to randomly access their
input or output (so that redirection via < or > won't suffice). The classic
example is tar(1); others abound. It is also used to indicate that arguments
normally taken from the command line should be taken from a file instead; see
awk(1) and egrep(1) for classic examples. Compare -o below; often, -f is the
input-side analog of -o.

Force (typically without argument). Force some operation (such as a file lock or
unlock) that is normally performed conditionally. This is less common.

Daemons often use -f in a way that combines these two meanings, to force
processing of a configuration file from a nondefault location. Examples: ssh(1),
httpd(1), and many other daemons.

-h
Headers (typically without argument). Enable, suppress, or modify headers on a
tabular report generated by the program. Examples: pr(1), ps(1).

Help. This is actually less common than one might expect offhand - for much of
Unix's early history developers tended to think of on-line help as
memory-footprint overhead they couldn't afford. Instead they wrote manual pages
(this shaped the man-page style in ways we'll discuss in Chapter 18).

-i
Initialize (usually without argument). Set some critical resource or database
associated with the program to an initial or empty state. Example: ci(1) in RCS.

Interactive (usually without argument). Force a program that does not normally
query for confirmation to do so. There are classical examples (rm(1), mv(1)) but
this use is not common.

-I
Include (with argument). Add a file or directory name to those searched for
resources by the application. All Unix compilers with any equivalent of
source-file inclusion in their languages use -I in this sense. It would be
extremely surprising to see this option letter used in any other way.

-k
Keep (without argument). Suppress the normal deletion of some file, message, or
resource. Examples: passwd(1), bzip(1), and fetchmail(1).

Occasionally -k has the sense of 'kill'.

-l
List (without argument). If the program is an archiver or interpreter/player for
some kind of directory or archive format, it would be quite surprising for -l to
do anything but request an item listing. Examples: arc(1), binhex(1), unzip(1).
(However, tar(1) and cpio(1) are exceptions.)

In programs that are already report generators, -l almost invariably means
"long" and triggers some kind of long-format display revealing more detail than
the default mode. Examples: ls(1), ps(1).

Load (with argument). If the program is a linker or a language interpreter, -l
invariably loads a library, in some appropriate sense. Examples: gcc(1), f77(1),
emacs(1).

Login. In programs such as rlogin(1) and ssh(1) that need to specify a network
identity, -l is how you do it.

Occasionally -l has the sense of 'length' or 'lock'.

-m
Message (with argument). Used with an argument, -m passes it in as a message
string for some logging or announcement purpose. Examples: ci(1), cvs(1).

Occasionally -m has the sense of 'mail', 'mode', or 'modification-time'.

-n
Number (with argument). Used, for example, for page number ranges in programs
such as head(1), tail(1), nroff(1), and troff(1). Some networking tools that
normally display DNS names accept -n as an option that causes them to display
the raw IP addresses instead; ifconfig(1) and tcpdump(1) are the archetypal
examples.

Not (without argument). Used to suppress normal actions in programs such as
make(1).

-o
Output (with argument). When a program needs to specify an output file or device
by name on the command line, the -o option does it. Examples: as(1), cc(1),
sort(1). On anything with a compiler-like interface, it would be extremely
surprising to see this option used in any other way. Programs that support -o
often (like gcc) have logic that allows it to be recognized after ordinary
arguments as well as before.

-p
Port (with argument). Especially used for options that specify TCP/IP port
numbers. Examples: cvs(1), the PostgreSQL tools, the smbclient(1), snmpd(1),
ssh(1).

Protocol (with argument). Examples: fetchmail(1), snmpnetstat(1).

-q
Quiet (usually without argument). Suppress normal result or diagnostic output.
This is very common. Examples: ci(1), co(1), make(1). See also the 'silent'
sense of -s.

-r (also -R)
Recurse (without argument). If the program operates on a directory, then this
option might tell it to recurse on all subdirectories. Any other use in a
utility that operated on directories would be quite surprising. The classic
example is, of course, cp(1).

Reverse (without argument). Examples: ls(1), sort(1). A filter might use this to
reverse its normal translation action (compare -d).

-s
Silent (without argument). Suppress normal diagnostic or result output (similar
to -q; when both are supported, q means 'quiet' but -s means 'utterly silent').
Examples: csplit(1), ex(1), fetchmail(1).

Subject (with argument). Always used with this meaning on commands that send or
manipulate mail or news messages. It is extremely important to support this, as
programs that send mail expect it. Examples: mail(1), elm(1), mutt(1).

Occasionally -s has the sense of 'size'.

-t
Tag (with argument). Name a location or give a string for a program to use as a
retrieval key. Especially used with text editors and viewers. Examples: cvs(1),
ex(1), less(1), vi(1).

-u
User (with argument). Specify a user, by name or numeric UID. Examples:
crontab(1), emacs(1), fetchmail(1), fuser(1), ps(1).

-v
Verbose (with or without argument). Used to enable transaction-monitoring, more
voluminous listings, or debugging output. Examples: cat(1), cp(1), flex(1),
tar(1), many others.

Version (without argument). Display program's version on standard output and
exit. Examples: cvs(1), chattr(1), patch(1), uucp(1). More usually this action
is invoked by -V.

-V
Version (without argument). Display program's version on standard output and
exit (often also prints compiled-in configuration details as well). Examples:
gcc(1), flex(1), hostname(1), many others. It would be quite surprising for this
switch to be used in any other way.

-w
Width (with argument). Especially used for specifying widths in output formats.
Examples: faces(1), grops(1), od(1), pr(1), shar(1).

Warning (without argument). Enable warning diagnostics, or suppress them.
Examples: fetchmail(1), flex(1), nsgmls(1).

-x
Enable debugging (with or without argument). Like -d. Examples: sh(1), uucp(1).

Extract (with argument). List files to be extracted from an archive or working
set. Examples: tar(1), zip(1).

-y
Yes (without argument). Authorize potentially destructive actions for which the
program would normally require confirmation. Examples: fsck(1), rz(1).

-z
Enable compression (without argument). Archiving and backup programs often use
this. Examples: bzip(1), GNU tar(1), zcat(1), zip(1), cvs(1).

The preceding examples are taken from the Linux toolset, but should be good on
most modern Unixes.

When you're choosing command-line option letters for your program, look at the
manual pages for similar tools. Try to use the same option letters they use for
the analogous functions of your program. Note that some particular application
areas that have particularly strong conventions about command-line switches
which you violate at your peril - compilers, mailers, text filters, network
utilities and X software are all notable for this. Anybody who wrote a mail
agent that used -s as anything but a Subject switch, for example, would have
scorn rightly heaped upon the choice.

The GNU project recommends conventional meanings for a few double-dash options
in the GNU coding standards.[104] It also lists long options which, though not
standardized, are used in many GNU programs. If you are using GNU-style options,
and some option you need has a function similar to one of those listed, by all
means obey the Rule of Least Surprise and reuse the name.

Portability to Other Operating Systems

To have command-line options, you have to have a command line. The MS-DOS family
does, of course, though in Windows it's hidden by a GUI and its use is
discouraged; the fact that the option character is normally '/' rather than '-'
is merely a detail. MacOS classic and other pure GUI environments have no close
equivalent of command-line options.
*/

#include "zoolib/ZCommandLine.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZUtil_Strim_Operators.h"
#include "zoolib/ZUtil_string.h"

#include <stdio.h>
#include <string.h> // For strlen

using std::exception;
using std::string;

namespace ZooLib {

// =================================================================================================
// MARK: - ZCommandLine

ZCommandLine* ZCommandLine::sCommandLineCurrent = nullptr;

ZCommandLine::ZCommandLine()
:	fHead(nullptr),
	fTail(nullptr)
	{
	sCommandLineCurrent = this;
	}

ZCommandLine::~ZCommandLine()
	{}

bool ZCommandLine::Parse(int argc, char** argv)
	{ return this->pParse(false, nullptr, argc, argv); }

bool ZCommandLine::Parse(const ZStrimW& iStrimErrors, int argc, char** argv)
	{ return this->pParse(false, &iStrimErrors, argc, argv); }

bool ZCommandLine::Parse(bool iUpdateArgs, int& ioArgc, char**& ioArgv)
	{ return this->pParse(true, nullptr, ioArgc, ioArgv); }

bool ZCommandLine::Parse(
	bool iUpdateArgs, const ZStrimW& iStrimErrors, int& ioArgc, char**& ioArgv)
	{ return this->pParse(true, &iStrimErrors, ioArgc, ioArgv); }

void ZCommandLine::WriteUsage(const ZStrimW& s) const
	{
	for (Opt* i = fHead; i; i = i->fNext)
		{
		if (i != fHead)
			s << " ";

		if (not i->fIsRequired)
			s << "[";

		s << i->fName;

		if (not i->fIsRequired)
			s << "]";
		}
	}

void ZCommandLine::WriteUsageExtended(const ZStrimW& s) const
	{
	this->WriteUsageExtended(s, "\t");
	}

void ZCommandLine::WriteUsageExtended(const ZStrimW& s, const string& iIndent) const
	{
	for (Opt* i = fHead; i; i = i->fNext)
		{
		s << iIndent << i->fName;

		if (i->fDescription.size())
			s << " \"" << i->fDescription << "\"";

		if (not i->fIsRequired)
			{
			if (i->fHasDefault)
				{
				s << " default: ";
				i->WriteDefault(s);
				}
			}

		s << "\n";
		}
	}

bool ZCommandLine::pParse(
	bool iUpdateArgs, const ZStrimW* iStrimErrors, int& ioArgc, char**& ioArgv)
	{
	// Assume we've been passed the line including the app name in position zero.
	int localArgc = ioArgc - 1;
	char** localArgv = ioArgv + 1;

	while (localArgc)
		{
		if (not this->pParseOne(iStrimErrors, localArgc, localArgv))
			return false;
		}

	for (Opt* i = fHead; i; i = i->fNext)
		{
		if (i->fIsRequired && !i->fHasValue)
			{
			if (iStrimErrors)
				*iStrimErrors << "Missing required option " << i->fName << "\n";
			return false;
			}
		}

	if (iUpdateArgs)
		{
		ioArgc = localArgc;
		ioArgv = localArgv;
		}
	return true;
	}

bool ZCommandLine::pParseOne(const ZStrimW* iStrimErrors, int& ioArgc, char**& ioArgv)
	{
	size_t argLength = strlen(ioArgv[0]);

	for (Opt* i = fHead; i; i = i->fNext)
		{
		size_t optLength = i->fName.size();
		if (optLength > argLength)
			continue;

		int match = i->fName.compare(0, optLength, ioArgv[0], optLength);
		if (match != 0)
			continue;

		const char* lexeme = nullptr;
		if (argLength > optLength)
			{
			if (ioArgv[0][optLength] != '=')
				continue;

			// It's opt followed by an equals sign.
			lexeme = ioArgv[0] + optLength + 1;
			ioArgv += 1;
			ioArgc -= 1;
			}
		else
			{
			// argLength and optLength must be the same.
			if (dynamic_cast<Boolean*>(i))
				{
				ioArgv += 1;
				ioArgc -= 1;
				}
			else
				{
				if (ioArgc < 2)
					{
					if (iStrimErrors)
						*iStrimErrors << "Missing parameter to option " << i->fName << "\n";
					return false;
					}
				lexeme = ioArgv[1];
				ioArgv += 2;
				ioArgc -= 2;
				}
			}

		if (i->fHasValue)
			{
			if (iStrimErrors)
				*iStrimErrors << "Option " << i->fName << " was repeated\n";
			return false;
			}

		return i->Parse(lexeme, iStrimErrors);
		}
	return false;
	}

void ZCommandLine::pAppendOpt(Opt* iOpt)
	{
	if (fTail)
		fTail->fNext = iOpt;
	else
		fHead = iOpt;
	fTail = iOpt;
	}

// =================================================================================================
// MARK: - ZCommandLine::Opt

ZCommandLine::Opt::Opt(
	const string& iName, const string& iDescription, EFlags iFlags, bool iHasDefault)
:	fNext(nullptr),
	fName(iName),
	fDescription(iDescription),
	fIsRequired(iFlags == eRequired),
	fHasValue(false),
	fHasDefault(iHasDefault)
	{
	ZAssert(ZCommandLine::sCommandLineCurrent);
	ZCommandLine::sCommandLineCurrent->pAppendOpt(this);
	}

ZCommandLine::Opt::~Opt()
	{}

bool ZCommandLine::Opt::HasValue() const
	{ return fHasValue; }

void ZCommandLine::Opt::WriteDefault(const ZStrimW& s)
	{}

// =================================================================================================
// MARK: - ZCommandLine::String

ZCommandLine::String::String(const string& iName, const string& iDescription, EFlags iFlags)
:	Opt(iName, iDescription, iFlags, false)
	{}

ZCommandLine::String::String(
	const string& iName, const string& iDescription, const string& iDefault)
:	Opt(iName, iDescription, eOptional, true),
	fDefault(iDefault)
	{}

ZCommandLine::String::String(const string& iName, const string& iDescription)
:	Opt(iName, iDescription, eOptional, false)
	{}

void ZCommandLine::String::WriteDefault(const ZStrimW& s)
	{ s << "\"" << fDefault << "\""; }

bool ZCommandLine::String::Parse(const char* iLexeme, const ZStrimW* iStrimErrors)
	{
	fValue = iLexeme;
	fHasValue = true;
	return true;
	}

const string& ZCommandLine::String::operator()() const
	{
	if (fHasValue)
		return fValue;
	return fDefault;
	}

// =================================================================================================
// MARK: - ZCommandLine::Boolean

ZCommandLine::Boolean::Boolean(const string& iName, const string& iDescription)
:	Opt(iName, iDescription, eOptional, false)
	{}

bool ZCommandLine::Boolean::Parse(const char* iLexeme, const ZStrimW* iStrimErrors)
	{
	if (iLexeme)
		{
		if (iStrimErrors)
			*iStrimErrors << "Unused parameter for bool option " << fName << "\n";
		return false;
		}
	fHasValue = true;
	return true;
	}

bool ZCommandLine::Boolean::operator()() const
	{
	return fHasValue;
	}

// =================================================================================================
// MARK: - ZCommandLine::Int64

ZCommandLine::Int64::Int64(const string& iName, const string& iDescription, EFlags iFlags)
:	Opt(iName, iDescription, iFlags, false),
	fValue(0)
	{}

ZCommandLine::Int64::Int64(const string& iName, const string& iDescription, int64 iDefault)
:	Opt(iName, iDescription, eOptional, true),
	fDefault(iDefault),
	fValue(0)
	{}

ZCommandLine::Int64::Int64(const string& iName, const string& iDescription)
:	Opt(iName, iDescription, eOptional, true),
	fValue(0)
	{}

void ZCommandLine::Int64::WriteDefault(const ZStrimW& s)
	{ s.Writef("%lld", fDefault); }

bool ZCommandLine::Int64::Parse(const char* iLexeme, const ZStrimW* iStrimErrors)
	{
	if (ZQ<int64> theQ = ZUtil_string::sQInt64(iLexeme))
		{
		fValue = theQ.Get();
		}
	else
		{
		if (iStrimErrors)
			*iStrimErrors << "Could not parse parameter to option " << fName << "\n";
		return false;
		}
	fHasValue = true;
	return true;
	}

int64 ZCommandLine::Int64::operator()() const
	{
	if (fHasValue)
		return fValue;
	return fDefault;
	}

} // namespace ZooLib
