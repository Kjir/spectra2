\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{unibo}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{report}}
\ProcessOptions
\LoadClass{report}

\usepackage{setspace}
%Define academic year
\newcommand\@acyear{}
\newcommand\acyear[1] {\renewcommand\@acyear{#1}}
%Define supervisor
\newcommand\@supervisor{}
\newcommand\supervisor[1] {\renewcommand\@supervisor{#1}}
%Define thesis subject
\newcommand\@thsubject{}
\newcommand\thsubject[1] {\renewcommand\@thsubject{#1}}
%Define session
\newcommand\@thsession{}
\newcommand\thsession[1] {\renewcommand\@thsession{#1}}

% Redefine \maketitle command
\renewcommand\maketitle{\begin{titlepage}
\begin{center}
{{\Large{\textsc{Alma Mater Studiorum $\cdot$ Universit\`a di
Bologna\\
Sede di Cesena}}}} \rule[0.1cm]{15.8cm}{0.1mm}
\rule[0.5cm]{15.8cm}{0.6mm}
{\small{\bf FACOLT\`A DI SCIENZE MATEMATICHE, FISICHE E NATURALI\\
Corso di Laurea in Scienze dell'Informazione}}
\end{center}
\vspace{15mm}
\begin{center}
\begin{doublespacing}
{\LARGE{\bf \MakeUppercase{\@title}}}\\
\end{doublespacing}
\vspace{19mm} {\large{\bf Relazione Finale in\\
\@thsubject}}
\end{center}
\vspace{40mm}
\par
\noindent
\begin{minipage}[t]{0.47\textwidth}
{\large{\bf Relatore:\\
Chiar.mo Prof.\\
\MakeUppercase{\@supervisor}}}
\end{minipage}
\hfill
\begin{minipage}[t]{0.47\textwidth}\raggedleft
{\large{\bf Presentata da:\\
\MakeUppercase{\@author}}}
\end{minipage}
\vspace{20mm}
\begin{center}
{\large{\bf Sessione \@thsession\\
Anno Accademico \@acyear}}
\end{center}
\end{titlepage}
}
\endinput
