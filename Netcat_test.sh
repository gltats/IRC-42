#!/bin/bash

# Open a netcat connection in the background
# Use -q 1 to quit 1 second after EOF
# Use -i 1 to wait 1 second between lines
nc -q 1 localhost 6969 -i 1 <<EOF &
PASS lol
NICK nickname

EOF

# Wait for the background netcat process to finish
wait

# Close the netcat connection
exec 3<&-

# exec 3<>/dev/tcp/localhost/6969

# printf "PASS lol \r\n" >&3
# printf "NICK nickname \r\n" >&3

# cat <&3

# exec 3<&-
# exec 3>&-