# tcf.agent
Target Communication Framework project repository (tcf.agent)

changes required by Movidius moviDebug2/tcf

This branch should be kept in sync with the moviDebug2 repository and with the `master` branch.

Sync process:

1. Pull all changes from https://github.com/eclipse/tcf.agent into this repository. Everything but especially `master` must be fast forward!
2. If required, manually `meld` changes from moviDebug2/tcf SVN TRUNK@HEAD into this branch. Mark SVN revision in commit message.
3. Merge master of this repository into this branch.
4. Manually meld changes from this branch into moviDebug2 SVN repository.
5. Create separate branch for integration if it's not trivial (i.e. moviDebug2 needs to be adapted)
6. Separate commits are preferred on the integration branch for separate stages of integration.
7. SVN TRUNK must have a single commit for the integration, with correctly working moviDebug2. Mark source commit ids and URLs. Review request mandatory before committing to SVN TRUNK.
8. Repeat **step 2** whenever non trivial changes were made to moviDebug2/tcf affecting common files.
