# Quantum Computing

_Scott Moonen, April 29, 1999_

Moore’s “law” has governed the advancement of computing technology for the past two decades, where its prescription of a doubling of speed and halving of size every 18 months has so far held reasonably true. If this empirical relationship continues to hold, in the very near future we will begin to reach the limits of what can be achieved with classical physics. As circuits grow small enough quantum effects begin to overwhelm the laws of electronics and the circuits cease to function.

All hope is not lost, however. Although quantum mechanics may prevent us from building circuits in the traditional manner, it lends itself to designing computers that function in different ways. Quantum computers rely on the principles of quantum mechanics rather than digital electronics to perform computation. This has very exciting prospects, since it turns out that computers built on quantum mechanics are superior to classical computers in more ways than simply being smaller and faster.

In order to understand the benefits of quantum computing it is first necessary to have an understanding of quantum mechanics, which we will explore first. After that will follow a brief introduction to the mathematics behind quantum computing, and then a discussion of the breakthroughs brought by quantum computing.

## Quantum Mechanics

If quantum mechanics were to be summarized in a single word, that word would most likely be _probability_. Quantum mechanics says that science can no longer know for certain every aspect of a physical system. Instead of pinpointing what _will_ happen, it can only describe what _may_ or may _not_ happen. Evolution of quantum mechanical probabilities is straightforward and deterministic,[^1] but the fact remains that physical properties can only be modeled by the evolution of probabilities,[^2] rather than certainties.

How are we to interpret these probabilities? Quantum mechanics tells us that when a system is not being observed, it in some sense exists in a _superposition_ of the states which its probability wave indicates it to be in. In other words, it is neither in one state, nor the other, but in some sense exists in all possible states simultaneously.

The physicist Erwin Schrödinger illustrated this idea with his famous mind experiment which has come to be called Schrödinger’s cat.[^3] Imagine a cat sealed inside a box, along with a bottle of ammonia. This bottle is connected to a Geiger counter which releases the ammonia into the box when it detects a particle of radiation. We cannot predict whether or when radiation will activate the Geiger counter, so we can only describe the _probability_ that the ammonia has been released and the cat killed. So in some sense the cat is both alive and dead at the same time![^4]

How do we reconcile this picture of reality, with everything existing in many states at once, with the very fixed and rigid reality we see every time we open our eyes? Quantum mechanical probabilities intersect with reality as we see it every time we make an _observation_. The act of observing a system causes it to _collapse_ into one of the states in which it is likely to be. Returning to Schrödinger’s cat, the cat is both alive and dead until we open the box, at which point it collapses into one or the other states. Which state it collapses into depends on the likelihood of it being in each state. If there is only a 1% probability of it being dead, then it will most likely collapse into the “alive” state when we open the box.

That’s all very well and nice: the universe does kooky things behind our back, but then scrambles back into a “normal” state whenever we look at it. How are we to know for sure that this is the way things are? Experiments have been devised that depend on a system being in several states, without our ever having to know which state it is in.

Consider the following experiment: imagine a source of particles which emits particles in the direction of a barrier. The barrier has two small slits in it through which the particles may pass, and behind the barrier is a viewscreen which registers the incidence of particles.

If we send individual particles from the source towards the barrier, some will inevitably pass through the slits and hit the viewscreen. One would expect that the distribution of flickers on the viewscreen would be two hazy spots centered around each of the slits in the barrier. This, however, is not the picture that appears.

What actually appears on the viewscreen is a bright spot in the center, between the two slits, surrounded by concentric rings of incident particles. This is a radical departure from what classical physics would predict. Quantum mechanics has an explanation for the situation, however. Since we don’t know which slit each individual particle passes through, its probability wave passes through _both_ slits, and in some sense the particle itself thus passes through both slits. As a result, the probability wave interferes with itself on the far side of the barrier. Thus, the alternating rings of high and low incidence rates, which indicate where the particle’s probability wave has constructively and destructively self–interfered, respectively.

Quantum mechanics also stipulates that there are certain things which we _can’t_ know by making an observation. For every measurement we may make, there is a corresponding _conjugate_ value whose state is influenced by our measurement of the first. The more certain we are of the value of a particular variable, the less certain we can be of the value of its conjugate. This is an inverse proportion; the product of the uncertainty in one variable (for example, a particle’s position) and the other variable (which, for position, is momentum) is bounded by a constant known as _Planck’s constant_. At the point where you become absolutely certain of a particle’s position, then its momentum is not pinned down at all; it could be traveling at any speed in any direction. Likewise, if you know the particle’s momentum with absolute certainty, it could be located anywhere in the universe.

## Quantum Computing

Quantum computing (QC) uses a quantum system to encode information in _quantum bits (qubits)_, rather than in classical bits. There are two ways in which qubits differ from classical bits:

1. A qubit can take on the value 0, 1, or some superposition of the two. It might, for example, be 60% one and 40% zero. When measured, it thus has a 60% chance of collapsing into a one, and a 40% chance of collapsing to zero.

2. A set of qubits may be _entangled_. In this situation, the value of one bit depends on the value of one or more other bits. For example, two bits may be entangled so that one of them must be 1 and the other 0. But until we observe the system, we don’t know which is 1 and which is 0. The moment we observe one of the qubits and find out whether it is 1 or 0, the other qubit also collapses into the opposite state, even though we haven’t observed it. This is because the qubits are _entangled_.

Entanglement is responsible for the difficulty of simulating a quantum system on a classical computer. For each additional qubit you add to a system, you are adding not an additional _pair_ of states, but you are actually doubling the number of states in the system, since the qubit may be entangled with any or all of the qubits already in the system. Herein also lies the advantage of quantum computers: through their ability to represent an exponential number of states they can solve certain exponential problems in polynomial time!

## Quantum Math

An individual qubit is typically portrayed as a unit vector in 2–space. The _x_–component represents the “zero–ness” of the qubit, and the _y_–component represents its “one–ness.” The probability that the qubit is zero or one is equal to the square of the vector’s component in the respective dimension. Vectors in quadrants other than the first have the same probabilities, but are said to be in a different _phase_, which does have an impact on the interaction of qubits in a quantum system but is incidental to a basic understanding.[^5]

For each additional qubit added to the system, the number of dimensions is doubled. For example, 3 qubits comprise a total of 8 dimensions, each representing the probability that the system is in a particular bit sequence (000, 001, 010, 011, 100, 101, 110, or 111). A set of qubits in a definite state is written as: `|state–value>`.

For example, a 3–bit system in the state 010 would be written as `∣010>` or `∣2>`. Systems in a superposition of states are written as the linear combination of state–value vectors, such as `(0.707∣010> + 0.707∣101>)` or `(0.8∣00> + 0.6∣11>)`, such that the sum of the coefficients adds to 1. The state of a system is represented as a column vector enumerating the coefficients for each state, respectively. For example, the system `(0.707∣010> + 0.707∣101>)` is represented as:

```
| 0     |
| 0     |
| 0.707 |
| 0     |
| 0     |
| 0.707 |
| 0     |
| 0     |
```

The notation `<state–value∣` describes a row–vector in the same form as the column vector given above. Thus, the product `<value1∣` * `∣value2>`, denoted `<value1∣value2>`, is 1 if value1 = value2, and 0 otherwise. This notation is sometimes used to denote a quantum transformation. The transformation H:

```
(0.707∣0> + 0.707∣1>)<0∣ + (0.707∣0> – 0.707∣1>)<1∣
```

yields the system `(0.707∣0> + 0.707∣1>)` when applied to the state `∣0>`, and yields the system `(0.707∣0> – 0.707∣1>)` when applied to the state `∣1>`. When applied to a state that is a superposition of `∣0>` and `∣1>`, the result is a combination of the two, depending on the coefficients of each component.

A more useful way of picturing a quantum transformation is as a matrix applied to the column vector representing a given state. By this means, the transformation H given above becomes the following 2×2 matrix:

```
| 0.707    0.707 |
| 0.707   –0.707 |
```

For an _n_–qubit system, the state matrix will be a 2<sup><i>n</i></sup>–dimensional column vector, and the transformation matrix will be a 2<sup><i>n</i></sup>×2<sup><i>n</i></sup> matrix. Not all possible matrices form valid quantum transformations. Only _unitary_ matrices are valid quantum transformations. These are matrices whose inverses are defined, and for which their inverse is equal to their conjugate transpose. What this implies is that all quantum computation is reversible; no information is lost in a quantum system.[^6]

## Implementation Model

Quantum computers encode qubits in various types of quantum systems, and manipulate them in corresponding ways. For example, information may be encoded in the polarization of photons. A popular abstract model for a quantum computer, however, is as a sequence of _traces_, much like electrical wires, upon which _quantum gates_ operate. A quantum gate is nothing more than an abstraction of a quantum transformation. Quantum circuits are built up out of a sequence of quantum gates.

It has been shown that four sets of quantum gates suffice to describe the behavior of any quantum system: C<sub>not</sub>, rotate, and two phase–shifts. C<sub>not</sub> is a conditional not; it operates upon two bits and inverts the value of the second bit only if the first bit has the value 1. Of course, “invert” and “has the value 1" take on quite different meanings for bits that are in a superposition of states. It has the following matrix representation:

```
| 1   0   0   0 |
| 0   1   0   0 |
| 0   0   0   1 |
| 0   0   1   0 |
```

Rotation simply rotates a qubit’s 2–space vector by a given angle θ. Its transformation matrix is as follows:

```
|  cos θ   sin θ |
| –sin θ   cos θ |
```

The two phase shifts operate in the complex domain to retain the system’s probabilities but shift its phase. Their transformation matrices are as follows:

<pre>| e<sup><i>i</i>θ</sup>   0  |
| 0    e<sup>–<i>i</i>θ</sup> |

| e<sup><i>i</i>θ</sup>   0  |
| 0    e<sup><i>i</i>θ</sup>  |</pre>

Together, these four transformations suffice to describe any quantum system.

## Quantum Computing

Now that we understand the fundamentals of the quantum mechanics underlying quantum computing, we are prepared to understand the advantages of quantum computing over classical computing. First, however, it must be pointed out that quantum computers are _at least_ as powerful as classical computers. The C<sub>not</sub> gate is sufficient to compute any classical Boolean logical operation. By exploiting transformations unique to quantum computing, however, and by using superposition, we can achieve more with quantum computers than with classical computers. There are three ways in which quantum computing outperforms classical computing:

1. Quantum computers can efficiently simulate a quantum system. Since a quantum computer _is_ a quantum system, it requires little overhead to simulate another quantum system. In order for a classical computer to simulate a quantum system, as already described, it must model an exponential number of variables for each quantum quantity in the system, which prevents simulation of all but the smallest and simplest of quantum systems.

2. Quantum computers can generate true random numbers. If a qubit is initialized to a state that is 50% zero and 50% one, then the reading of that qubit generates a true random bit, which can be used to construct a random number. Classical computers, on the other hand, can only generate pseudo–random numbers, which appear to be random but in reality follow some pattern. This also applies to the previous point; a classical computer uses pseudo–random numbers to simulate a quantum system, and in some sense is thus a slightly inadequate representation of a quantum system. Depending on the quality of the random number generator, it may produce skewed results.

3. Quantum computers can exploit their advantage of superposition to achieve significant speed–up in computation. For example, Grover’s linear search algorithm is able to achieve a linear search in O(√<i>n</i>) time, a significant improvement over the optimal O(_n_) for a classical computer. Even more impressive, quantum computers can tackle certain exponential problems in polynomial time. Shor’s algorithm for factoring integers succeeds in factoring numbers in polynomial time. No algorithms better than exponential ones are known for the factoring problem for classical computers. The burning question is whether NP–complete problems are polynomial–time for a quantum computer, a question which remains unresolved.

The ability of a quantum computer to achieve a substantial speed–up over a classical computer arises from superposition. By feeding a superposition of states into a quantum circuit that computes a function, we are essentially evaluating that function simultaneously on all given inputs. Unfortunately, we aren’t able to directly to exploit this, since the _output_ of the function is also a superposition: by reading it off, we can only grab one of the values of the function. We don’t necessarily need to be able to read off all the output values, however. Transformations can be applied to the result to compute useful aggregate statistics on the function’s output. One of the more common techniques is to apply the Fourier transform to the superimposed results to transform them into the frequency domain. This allows us to determine features of the output such as periodicity.

Unfortunately, our ability to feasibly exploit the advantages of quantum computing is severely limited. The best quantum computers built to date contain only a handful of qubits and are severely affected by environmental interference. Useful quantum applications require quantum computers with several hundred or more qubits, which unfortunately seems at least 50–100 years in the future.

## Quantum Programming

For the programming aspect of my project, I implemented what I believe to be the first quantum programming language. This language is versatile enough to generate simple quantum simulations for classical computers, yet is powerful enough to drive controller hardware for a full–featured quantum computer. I wrote a compiler for my language with the help of the UNIX tools `lex` and `yacc`. The compiler generates a transformation matrix representing the quantum circuit being modeled.

The syntax for my language is fairly straightforward, and is somewhat reminiscent of the C class of languages. Quantum circuits are described as function calls. Each function may in turn invoke sub–functions. Functions may take a variable number of parameters, and may thus be extensible to variable–size quantum computers. As a result, my language implements conditional statements and loops to assist in handling a variable number of parameters. A sample function definition follows:

```
foo(x; y[m]; z[n])
{
  bar(x,; y[0]);
  if(m = 1)
  {
    sally(y[0], x; z);
  }
  else
  {
    for i, 0, m–1
    {
      for j, 0, n–1
      {
        joe(x, y[i]; x, z[i]);
      }
    }
  }
}
```

Notice that function parameters are separated both by commas and by semicolons. This is to distinguish individual parameters from aggregate parameters or parameter groups. This facilitates the grouping of parameters into several variable–size parameter arrays.

## Conclusion

Although practical use of quantum computing lies still fairly far in the future, it holds great promise for revolutionizing the fields of computer science and mathematics. Perhaps several decades from now a quantum programming course will be standard freshman fare in the computer science curriculum. Until then, it is important to continue exploring quantum computing, to try to understand it better and perhaps discover additional algorithmic advances permitted by this exciting field.

## Bibliography

Appel, Andrew W. _Modern Compiler Implementation in Java_.

Levine, John R.; Mason, Tony; and Brown, Doug. _`lex` & `yacc`_.

Rieffel, Eleanor, and Polak, Wolfgang. _An Introduction to Quantum Computing for Non–Physicists_. http://xxx.lanl.gov/abs/quant-ph/?9809016

Steane, Andrew. _Quantum Computing_. http://xxx.lanl.gov/abs/quant-ph/?9708022

Williams, Colin. _Explorations in Quantum Computing_.

## Endnotes

[^1] Despite the non–linearity of many classical physical systems, it is interesting to note that the chaos of reality is built upon a foundation of _linear_ quantum systems.

[^2] Probability evolution takes place through the evolution of a _probability wave_, which expresses the probability that a system is in a certain state. For example, a particle has a waveform describing the probability that it is at various locations.

[^3] Cat lovers may rest easy–no cats were harmed in the describing of this experiment.

[^4] Depending, of course, on the probability of how many of the cat’s nine lives remain unspent.

[^5] This description is also simplified in that the vector components are all really complex numbers, but the vector itself must still be a unit vector. The probabilities for each value become the square of the norm of each component.

[^6] This is, of course, assuming an ideal noise–free system. Interesting work has been done with classical reversible computing, which avoids the entropy increase of discarding input values. This allows a system to function more efficiently, dissipating far less energy. Quantum systems already have this wonderful low–dissipation feature!

