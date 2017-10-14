---
title: "CoAPing with the Internet of Things"
author: Anmol Sarma \<me@anmolsarma.in\>
subtitle: RFC 7252
institute: Huawei
output: beamer_presentation
---

# Agenda

- What
- Why
- How
- Demo!

# CoAP

>  "The Constrained Application Protocol (CoAP) is a specialized web transfer protocol for use with **constrained nodes** and **constrained networks**." 
>
>  -- *RFC 7252*

---------------

# Constrained-Node Networks

> Obligatory `RFC 7228` reference...

. . . 

Basically:

 - Small
 - Slow
 - Lossy

 . . . 

 **Think more Arduino, less RaspberryPi**

# Internet of Things

Currently:

 - **WWW**: TCP + (TLS) + HTTP
 - *Very* widely used and *very* familiar

. . .

IoT? IoE? WoT? &lt;Insert More Buzzwords/&gt;

. . .

 - Something more lightweight
 - Something *semantically* compatible

# RESTful

. . .

> Pause for rants, flames and holy wars...

. . .

- Client-Server
- Stateless
- Cache
- Layered System
- Uniform Interface

. . .

**Hypermedia as the Engine of Application State**

# Constrained RESTful Environments (core)

> "... provides a framework for resource-oriented applications intended to
> run on constrained IP networks."

> "... the working group has defined a Constrained Application Protocol (CoAP) for the manipulation of Resources on a Device."


# CoAP Features

- **Asynchronous** request-response model
- Methods analogous to HTTP (GET, PUT, POST)
- Response codes similar to HTTP (2.05, 4.03, 5.00)
- Options in place of headers
- MIME types (JSON, CBOR)
- URI support: `coap://[HOST]:PORT/path?q=query`
- Compact binary protocol (4 byte header)


- UDP binding (TCP and SMS bindings also possible)
- **Observation i.e. subscription mechanism**
- **Resource discovery**
- Block transfer

# Semantics of CoAP methods

> ... are "almost, but not entirely unlike" [HHGTTG] those of HTTP methods.

Message Types:

- **CON**firmable
- **NON**-confirmable
- **ACK**nowledgment
- **RST**

# Piggybacked Response

\centerline{\includegraphics[height=1.8in]{req_rsp.png}}

# Deferred (Async) Response

\centerline{\includegraphics[height=2in]{deferred_rsp.png}}

# Observe

\centerline{\includegraphics[height=2in]{observe.png}}

# Resource Discovery RFC 6690

. . .

    REQ: GET /.well-known/core

. . .

    RES: 2.05 Content
    </sensors>;ct=40

. . .

    REQ: GET /sensors

. . .

    RES: 2.05 Content
    </sensors/temp>;rt="temperature-c";if="sensor",
    </sensors/light>;rt="light-lux";if="sensor"


# Resource Directory (Draft)

. . . 

\centerline{\includegraphics[height=1.8in]{rd.png}}

. . .

- **Cache / Reverse Crawler** 

---------------------------------

## Application Profiles

- OMA LWM2M
- IPSO Alliance
- IETF CoRE Interfaces (Draft)

## Implementations

- `http://coap.technology/impls.html`

# Message Format

\centerline{\includegraphics[height=1.4in]{msg.png}}

# Option Format

\centerline{\includegraphics[height=2in]{opt.png}}

# Demo!

# ?? || //



