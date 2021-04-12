
# https://stackoverflow.com/a/40222538/795574

# exit_register runs at the end of ipython %run or the end of the python interpreter
try:
    def exit_register(fun, *args, **kwargs):
        """ Decorator that registers at post_execute. After its execution it
        unregisters itself for subsequent runs. """
        def callback():
            fun()
            ip.events.unregister('post_execute', callback)
        ip.events.register('post_execute', callback)


    ip = get_ipython()
except NameError:
    from atexit import register as exit_register


# @exit_register
# def callback():
#     print('I\'m done!')


