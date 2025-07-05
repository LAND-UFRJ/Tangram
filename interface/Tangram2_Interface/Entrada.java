/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

package Tangram2;

public class Entrada
{
  public native int get_precision(String precision_str);
  public native int get_partition(String partition_str, String basename);
  public native int get_intervals(String intervals_str, String basename);
  public native int get_initial_probs(String initial_probs_str, String basename);

  Entrada ()
  {
     /* construtor */

      System.loadLibrary("Input_parser");
  }

  public int precision (String precision_str)
  {
     return (get_precision(precision_str));
  }

  public int partition (String partition_str, String basename)
  {
     return (get_partition(partition_str, basename));
  }

  public int intervals (String intervals_str, String basename)
  {
     return (get_intervals(intervals_str, basename));
  }

  public int initial_probs (String initial_probs_str, String basename)
  {
     return (get_initial_probs(initial_probs_str, basename));
  }
}

// ------

// Entrada e = new Entrada();

// e.precision ("...");
