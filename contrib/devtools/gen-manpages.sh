#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

NEOXAD=${NEOXAD:-$SRCDIR/neoxad}
NEOXACLI=${NEOXACLI:-$SRCDIR/neoxa-cli}
NEOXATX=${NEOXATX:-$SRCDIR/neoxa-tx}
NEOXAQT=${NEOXAQT:-$SRCDIR/qt/neoxa-qt}

[ ! -x $NEOXAD ] && echo "$NEOXAD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
NEOXVER=($($NEOXACLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for neoxad if --version-string is not set,
# but has different outcomes for neoxa-qt and neoxa-cli.
echo "[COPYRIGHT]" > footer.h2m
$NEOXAD --version | sed -n '1!p' >> footer.h2m

for cmd in $NEOXAD $NEOXACLI $NEOXATX $NEOXAQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${NEOXVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${NEOXVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
