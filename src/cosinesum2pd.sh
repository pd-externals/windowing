#!/bin/sh


usage() {
    cat >/dev/stderr <<EOF
usage: $0 <name> a0 a1 ... aK

       creates a Pd patch for the cosinesum-window <name>
       using the formula:
       w[n] = sum{k=0..K}{(-1)^k * a_k * cos (2π*k*n/N)}

EOF

    exit 1
}

template() {
    cat <<EOF
#N canvas 100 100 450 175 12;
#X obj 36 66 inlet~;
#X obj 36 116 outlet~;
#X obj 36 91 windowing/cosinesum~ ${coefficients};
#X text 32 30 ${name} window: ${formula};
#X text 123 124 © ${year} \, IOhannes m zmölnig <zmoelnig@iem.at>;
#X text 131 145 GPL-2.0-or-later;
#X connect 0 0 2 0;
#X connect 0 1 2 0;
#X connect 2 0 1 0;
EOF
}

togglesigns() {
    local x
    local i
    local sign
    i=0
    sign="+"
    for x in "$@"; do
        i=$((i+1))
        if [ "${sign}" = "+" ]; then
            sign="-"
        else
            sign="+"
        fi
        x="$(printf "%g" "$(echo "0 ${sign} (${x})" | bc)")"

        if [ "${x#-}" = "${x}" ]; then
            x="+ ${x}"
        else
            x="- ${x#-}"
        fi
        if [ "${i}" = 1 ]; then
            printf " %s" "$x*cos(π*x)"
        else
            printf " %s" "$x*cos(${i}π*x)"
        fi
    done
}


year="$(date +%Y)"
name="$1"

[ -n "${name}" ] || usage
shift

coefficients="$*"
[ -n "$*" ] || usage

c0="$(printf '%g' "$1")"
shift

formula="${c0}$(togglesigns "$@")"


template
