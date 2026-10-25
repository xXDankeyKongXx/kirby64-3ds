#!/bin/bash
# Runs a command; on failure, surfaces errors + log tail as a GitHub annotation.
log=$(mktemp)
bash -ec "$1" 2>&1 | tee "$log"
rc=${PIPESTATUS[0]}
if [ "$rc" -ne 0 ]; then
	{ echo "== error lines =="; grep -iE "error|undefined reference|not found|No such file" "$log" | head -40
	  echo "== last lines =="; tail -n 50 "$log"; } > "$log.sum"
	msg=$(sed 's/%/%25/g; s/\r//g' "$log.sum" | awk '{printf "%s%%0A", $0}')
	echo "::error title=Step failed::$msg"
fi
exit "$rc"
