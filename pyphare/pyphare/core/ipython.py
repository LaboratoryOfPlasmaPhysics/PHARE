
# https://stackoverflow.com/a/40222538/795574

try:
    def exit_register(fun, *args, **kwargs):
        def callback():
            fun()
            ip.events.unregister('post_execute', callback)
        ip.events.register('post_execute', callback)

    ip = get_ipython() # should fail if not running under ipython

except NameError:
    # forwarding import to keep functionality even if ipython is not in use
    #  it's possible this isn't even needed, and we just want to execute the callback for only if
    #    we're running under ipython, in that case, "exit_register" -> "ipython_exit_register" and
    #     replace the next line with def ipython_exit_register(fun, *args, **kwargs): pass
    from atexit import register as exit_register # lgtm [py/unused-import]


