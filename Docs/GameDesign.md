# Game Design — core direction

Turn-based tactical military game. Two advertised selling points: adaptive AI and
reconnaissance-centric gameplay.

## The pillar

The two selling points are one pillar seen from both ends: **information asymmetry, running in
both directions.** The player scouts to learn about the enemy; the enemy observes to learn about
the player. Every feature should be judged against whether it makes information more valuable,
more perishable, or more contested.

Treating adaptation and recon as one pillar is also what makes the adaptive AI survivable as a
feature — knowledge is already the currency the player thinks in, so an AI that pays in the same
currency reads as an opponent rather than as a handicap system.

## The fairness rule

> **The AI adapts by spending the same currency the player spends — knowledge and preparation.
> Never stats.**

Adaptive AI feels like cheating when adaptation is invisible and numerical: accuracy creeps up,
HP inflates, reaction times shorten. The player cannot see it, scout it, or counter it, so it
reads as the game weighting the scale. That is rubber-banding.

Compare:

- *"You have breached through windows four missions running, so this squad boarded the windows
  and posted a watcher upstairs."* — visible, scoutable, counterable, diegetic.
- *"This squad has +15% aim because you are winning."* — none of those.

Same design intent, opposite player experience.

## Adaptation timescales

Three categories. Build the first two, never build the third.

**Tactical (within a mission).** The AI notices a flank developing and repositions. Low fairness
risk — reads as competence rather than adaptation. Implemented as behaviour weights inside the
brain.

**Strategic (across missions).** The enemy changes its *preparation*: boards windows, shifts
patrols, sets an ambush where the player habitually enters. This is the actual selling point, and
it is the **safer** of the two for fairness, because preparation is a physical fact in the level
that the player can scout before committing. Implemented as level setup, not combat math.

**Resolution-level (forbidden).** Anything that touches hit rolls, damage, or action economy in
response to player performance. Most of the fairness risk lives here and nowhere else.

## Witnesses — the learning channel

**The AI may only learn from what its units observed and survived to report.**

One constraint, disproportionate returns:

- **Fair by construction.** Every adaptation traces to a specific enemy who saw a specific thing
  and escaped. The player could have prevented it.
- **Creates a real objective.** "No survivors" becomes strategy rather than flavour — the player
  is denying intelligence, not farming kills.
- **Makes recon bidirectional.** Being seen carries a persistent cost, not merely an immediate
  one. This is what makes stealth matter beyond the current turn.
- **Lets the player lie.** Deliberately allowing a witness to escape after atypical behaviour
  poisons the enemy's model. **Feinting** — a tactics game where the player can feed the AI false
  intelligence, arising directly from the pillar without inventing new systems.

Close the loop by **narrating adaptation back to the player**. Post-mission: *"Enemy command
noted: window entry, 4 of 5 engagements."* Transparency converts "unfair" into "respect" — the
Nemesis system worked because the game told the player the orcs remembered them, not merely
because they did.

## Fog of war — decay, not absence

The failure mode of fog in turn-based tactics is re-scouting tedium. If vision is expensive to
hold and free to lose, players check every corner every turn and the game becomes chores.

**Information should decay rather than vanish.** Last-known-position ghosts, timestamped —
"seen here, 2 turns ago". A decaying marker gives the player something to reason *with* and makes
predicting enemy movement a skill. A black tile gives them only anxiety.

## Vision is symmetric

If the player can see an enemy through a window, that enemy can potentially see the player.

Build this from day one rather than retrofitting. Symmetric vision is what turns reconnaissance
from a free resource into a risk, and it is the precondition for the witness mechanic — the AI
cannot learn from observation if observation only runs one way. Asymmetric vision is the kind of
assumption that calcifies into every later system.

## First level — the window house

One enemy inside a house with a door and a window. The AI does nothing but shoot anyone who
enters. The house interior is hidden. Intended solution: gain line of sight through the window
and shoot first, rather than entering through the door.

**What it teaches:** vision is not entry. Position determines knowledge; knowledge determines who
shoots first.

**Known limitation:** as specified, peeking the window strictly dominates — no cost, no risk,
therefore no decision. Acceptable for a teaching level provided that is understood to be its
purpose. Level 2 must introduce the tension. Cheapest ways to add cost when the time comes:

- Peeking consumes action points, so the shot happens with fewer remaining.
- The window is exposed to a second angle the player must cross to reach it.
- The window reveals only a cone — the player learns where the enemy *is not*.

## Open decisions

**Squad or single unit.** Currently single-unit: `Control` carries one `active` flag and
`PlayerControl` resolves a single `_me`. A recon-centric design argues for a small squad (2–4),
because the central tension of reconnaissance is **splitting vision from firepower** — the scout
who sees cannot shoot, the shooter cannot see. A single unit collapses that into one position and
loses the most interesting decision the genre offers. Invisible Inc. runs 2–3 agents for this
reason.

**Scope reality.** Cover, line of sight, and fog of war are three systems, and there are
currently no obstacles at all — `getReachableTiles` is a Manhattan disc with no wall awareness
(`World.cpp:120`). The window house additionally needs wall tiles, LOS raycasting with the usual
tile-corner ambiguity, cover flags, per-unit vision state, and fog rendering. This is substantial
engine work occurring *after* the intended stop-doing-engine-work point. That is consistent with
the plan — design pulling features — but it means "start game design" opens with a long stretch of
engine.

## Reference games

- **Invisible Inc.** — closest existing game to this direction; built on information decay and
  last-known-position reasoning. Study first.
- **Frozen Synapse** — vision-driven simultaneous planning.
- **Door Kickers** — breach planning and entry-point choice.
- **Phantom Doctrine** — counterintelligence framing.
- **Shadow of Mordor** — Nemesis system; the reference for narrating adaptation back to the
  player.
