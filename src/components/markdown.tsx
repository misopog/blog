import ReactMarkdown from 'react-markdown'
import remarkGfm from 'remark-gfm'

interface MarkdownProps {
  children: string
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
        pre: ({ children }: { children: React.ReactNode }) => (
          <pre className="bg-zinc-950 text-gray-200 rounded-lg p-4 mb-4 overflow-x-auto">
            {children}
          </pre>
        ),
      }}
    >
      {children}
    </ReactMarkdown>
  )
}