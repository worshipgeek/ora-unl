# ora-unl
Oracle Unload Utility

A command line tool that takes, at a minimum, a SQL select statement (of any complexity) and produces a OS-native flatfile representation of the results.  There are a large number of options available to tweak the format of the output, to produce fixed-width or delimited records.  Please eee unl.1 for a MAN page of all the available options.

The Makefile may need to be modified to get the code to link, since with each version of Oracle the library dependencies change slightly.
