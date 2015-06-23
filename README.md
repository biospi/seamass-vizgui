seamass-viz
===========

Streaming visualisation of SMV files created by seaMass 

building
-------
depends on _Boost_, _Eigen_ and _libspatialindex_. **Note: _libspatialindex_ MUST be at least version 1.8.5**. Windows and Mac developers can get these from the [dependency repository](https://github.com/biospi/seamass-windeps).

For compile under visual studio 2010 add x64 platform to:
Configuration Manager->Active Solution Platform 
to x64
And all Project platform (on same dialogue) to x64.

Also delete "Additional Options" under:
vizGUI->properties->Configuration Properties->Linker->commandline

running
-------
Please refer to the included [tutorial](docs/Tutorial.pdf) for usage instructions.

Change
Solution->Properties->CommonO= Properties->Single startup project
from ALL_BUILD to vizGUI