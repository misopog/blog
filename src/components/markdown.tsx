"use client";

import ReactMarkdown from 'react-markdown'
import remarkGfm from 'remark-gfm'
import { useState } from 'react'

interface MarkdownProps {
  children: string
}

function extractTextFromReactNode(node: React.ReactNode): string {
  if (typeof node === 'string') {
    return node;
  }
  if (Array.isArray(node)) {
    return node.map(extractTextFromReactNode).join('');
  }
  if (typeof node === 'object' && node) {
    return extractTextFromReactNode((node as any).props.children);
  }
  return '';
}

function Pre({ children }: { children: React.ReactNode }) {
  const [copied, setCopied] = useState(false)

  const handleCopy = () => {
    const text = extractTextFromReactNode(children);
    navigator.clipboard.writeText(text)
    setCopied(true)
    setTimeout(() => setCopied(false), 2000)
  }

  return (
    <div className="relative">
      <button
        onClick={handleCopy}
        className="absolute right-2 top-2 bg-gray-800 text-white px-2 py-1 text-xs rounded"
      >
        {copied ? 'Copied!' : 'Copy'}
      </button>
      <pre className="bg-zinc-950 text-gray-200 rounded-lg p-4 mb-4 overflow-x-auto">
        {children}
      </pre>
    </div>
  )
}

export function Markdown({ children }: MarkdownProps) {
  return (
    <ReactMarkdown
      remarkPlugins={[remarkGfm]}
      components={{
        h1: ({ children }) => (
          <h1 className="text-3xl font-bold mt-8 mb-4">{children}</h1>
        ),
        h2: ({ children }) => (
          <h2 className="text-2xl font-bold mt-6 mb-3">{children}</h2>
        ),
        p: ({ children }) => (
          <p className="mb-4 leading-relaxed">{children}</p>
        ),
        a: ({ href, children }) => (
          <a href={href} className="text-blue-400 hover:underline">
            {children}
          </a>
        ),
        ul: ({ children }) => (
          <ul className="list-disc pl-6 mb-4">{children}</ul>
        ),
        ol: ({ children }) => (
          <ol className="list-decimal pl-6 mb-4">{children}</ol>
        ),
        code: ({ children }: { children: React.ReactNode }) => (
          <code className="bg-zinc-950 text-gray-200 rounded-lg px-2 py-1">{children}</code>
        ),
        pre: Pre,
        table: ({ children }) => (
          <div className="overflow-x-auto mb-4">
            <table className="min-w-full border-collapse border border-gray-300">
              {children}
            </table>
          </div>
        ),
        thead: ({ children }) => (
          <thead className="bg-gray-100">{children}</thead>
        ),
        tbody: ({ children }) => (
          <tbody>{children}</tbody>
        ),
        tr: ({ children }) => (
          <tr className="border-b border-gray-300">{children}</tr>
        ),
        th: ({ children }) => (
          <th className="border border-gray-300 px-4 py-2 text-left font-semibold">{children}</th>
        ),
        td: ({ children }) => (
          <td className="border border-gray-300 px-4 py-2">{children}</td>
        ),
      }}
    >
      {children}
    </ReactMarkdown>
  )
}