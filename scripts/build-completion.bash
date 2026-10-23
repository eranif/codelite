# Bash completion for CodeLite's build.sh
#
# Usage: add this line to your ~/.bashrc
#
#   source /path/to/codelite/scripts/build-completion.bash
#
# Keep the lists below in sync with the options and targets of build.sh

_codelite_build_sh() {
    local cur="${COMP_WORDS[COMP_CWORD]}"
    local opts="--cmake --tests --no-build --opengl -h --help"
    local targets="debug clean distclean package wxcrafter package_wxcrafter"

    if [[ "${cur}" == -* ]]; then
        COMPREPLY=($(compgen -W "${opts}" -- "${cur}"))
        return 0
    fi

    # Only one target is allowed, do not offer any if one was already typed
    local word target
    for word in "${COMP_WORDS[@]:1:COMP_CWORD-1}"; do
        for target in ${targets}; do
            if [[ "${word}" == "${target}" ]]; then
                COMPREPLY=()
                return 0
            fi
        done
    done
    COMPREPLY=($(compgen -W "${targets}" -- "${cur}"))
}

complete -F _codelite_build_sh build.sh ./build.sh
