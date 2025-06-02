## Colt Express Communication API

This is code taken from the main repository.
- Under `Scripts\`, `SelectManager.cs` is an example of the API being called when a player picks a character during the game.
- Under `Scripts\`, `NamedClient.cs` contains the code for streaming the data to / parsing the bytes received from the server.

`Scripts\ServerToClient` contains the main bulk of the API - the API itself, the API handler (which parses the JSON message to determine the event only), the event manager, and an extensible base class for listeners.
- The `Listener` and `Object Classes` folders contain most of the listeners used for the UI, and some very small classes for holding relevant game data.

`Scripts\MainGame\UI` contains a sample of some of the code I wrote for the UI. A round of Colt Express has two phases - an initial phase where the player plays cards/takes actions, and a second phase resolving those actions. `ScheminPhaseManager.cs` handled the initial phase.