ls; ls

(echo -e "2\n1" | sort > list12) &
(echo -e "1\n3" | sort > list13) &

wait   # Don't execute the next command until subshells finish.

diff list12 list13

