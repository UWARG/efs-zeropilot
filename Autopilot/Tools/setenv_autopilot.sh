#!/bin/bash

# local variables
USER=$(whoami)
AUTOPILOT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
AUTOPILOT_ENVSHELL="/bin/bash"
AUTOPILOT_ENVRC_SRC="autopilot.envrc"
AUTOPILOT_ENVRC="/tmp/autopilot_${USER}_bashrc"

# subshell to create the environment's rc file
(
  echo "#!/bin/bash"
  echo "export AUTOPILOT_ROOT=\"$AUTOPILOT_ROOT\""
  echo ""
  echo "# ====== START OF ENVIRONMENT AUTOINSERT ======"
  cat  "$AUTOPILOT_ROOT/$AUTOPILOT_ENVRC_SRC"
  echo "# ====== END OF ENVIRONMENT AUTOINSERT ======"
) > $AUTOPILOT_ENVRC

# output status
echo ""
echo "Switching to ZeroPilot Autopilot development environment...";
echo ""

# start environment
exec $AUTOPILOT_ENVSHELL --rcfile $AUTOPILOT_ENVRC
